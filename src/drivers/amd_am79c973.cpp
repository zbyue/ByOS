//
// Created by zbyue on 3/29/21.
//

#include "../../include/drivers/amd_am79c973.h"
#include "../../include/common/io.h"
using namespace byos;
using namespace byos::common;
using namespace byos::drivers;
using namespace byos::hardware;

amd_am79c973::amd_am79c973(PciDeviceDescriptor *dev, InterruptManager* interruptManager)
:   Driver(),
    InterruptHandler(dev->interrupt + interruptManager->HardwareInterruptOffset(), interruptManager),
    MACAddress0Port(dev->portBase),
    MACAddress2Port(dev->portBase + 0x02),
    MACAddress4Port(dev->portBase + 0x04),
    registerDataPort(dev->portBase + 0x10),
    registerAddressPort(dev->portBase + 0x12),
    resetPort(dev->portBase + 0x14),
    busControlRegisterDataPort(dev->portBase + 0x16)
{
    currentSendBuffer = 0;
    currentRecvBuffer = 0;

    uint64_t MAC0 = MACAddress0Port.Read() % 256;
    uint64_t MAC1 = MACAddress0Port.Read() / 256;
    uint64_t MAC2 = MACAddress2Port.Read() % 256;
    uint64_t MAC3 = MACAddress2Port.Read() / 256;
    uint64_t MAC4 = MACAddress4Port.Read() % 256;
    uint64_t MAC5 = MACAddress4Port.Read() / 256;

    uint64_t MAC = MAC5 <<40
                    | MAC4 << 32
                    | MAC3 << 24
                    | MAC2 << 16
                    | MAC1 << 8
                    | MAC0;

    // 32 bit mode
    registerAddressPort.Write(20);
    busControlRegisterDataPort.Write(0x102);

    // STOP reset
    registerAddressPort.Write(0);
    registerDataPort.Write(0x04);

    // initBlock
    initBlock.mode = 0x0000; // promiscuous mode = false
    initBlock.reserved1 = 0;
    initBlock.numSendBuffers = 3;
    initBlock.reserved2 = 0;
    initBlock.physicalAddress = MAC;
    initBlock.reserved3 = 0;
    initBlock.logicalAddress = 0;

    sendBufferDesc = (BufferDescriptor*)(((uint32_t)(&sendBufferDescMemory[0]) + 15) & ~((uint32_t)0xF));
    initBlock.sendBufferDescAddress = (uint32_t)sendBufferDesc;

    recvBufferDesc = (BufferDescriptor*)(((uint32_t)(&recvBufferDescMemory[0]) + 15) & ~((uint32_t)0xF));
    initBlock.recvBufferDescAddress = (uint32_t)recvBufferDesc;

    for(uint8_t i = 0; i < 8; ++i)
    {
        sendBufferDesc[i].address = (((uint32_t)&sendBuffers[i]) + 15) & ~(uint32_t)0xF;
        sendBufferDesc[i].flags = 0x7FF
                                | 0xF000;
        sendBufferDesc[i].flags2 = 0;
        sendBufferDesc[i].avail = 0;

        recvBufferDesc[i].address = (((uint32_t)&recvBuffers[i]) + 15) & ~(uint32_t)0xF;
        recvBufferDesc[i].flags = 0xF7FF
                                | 0x80000000;
        recvBufferDesc[i].flags2 = 0;
        recvBufferDesc[i].avail = 0;

    }
    registerAddressPort.Write(1);
    registerDataPort.Write((uint32_t)(&initBlock) & 0xFFFF);
    registerAddressPort.Write(2);
    registerDataPort.Write(((uint32_t)(&initBlock) >> 16) & 0xFFFF);

}
amd_am79c973::~amd_am79c973()
{

}

void amd_am79c973::Activate()
{
    registerAddressPort.Write(0);
    registerDataPort.Write(0x41);

    registerAddressPort.Write(4);
    uint32_t temp = registerDataPort.Read();
    registerAddressPort.Write(4);
    registerDataPort.Write(temp | 0xC00);

    registerAddressPort.Write(0);
    registerDataPort.Write(0x42);
}
int amd_am79c973::Reset()
{
    resetPort.Read();
    resetPort.Write(0);
    return 10;
}

uint32_t amd_am79c973::HandleInterrupt(uint32_t esp)
{
    printf("INTERRUPT FROM AMD am79c973\n");
    registerAddressPort.Write(0);
    uint32_t temp = registerDataPort.Read();

    if((temp & 0x8000) == 0x8000) printf("AMD am79c973 ERROR\n");
    if((temp & 0x2000) == 0x2000) printf("AMD am79c973 COLLISION ERROR\n");
    if((temp & 0x1000) == 0x1000) printf("AMD am79c973 MISSED FRAME\n");
    if((temp & 0x0800) == 0x0800) printf("AMD am79c973 MEMORY ERROR\n");
    if((temp & 0x0400) == 0x0400) receive();
    if((temp & 0x0200) == 0x0200) printf("AMD am79c973 DATA SENT\n");

    registerAddressPort.Write(0);
    registerDataPort.Write(temp);
    if((temp & 0x0100) == 0x0100) printf("AMD am79c973 INIT DONE\n");

    return esp;
}

void amd_am79c973::send(uint8_t* buffer, int size)
{
    printf("Send");
    //printf("\n");
    int sendDescriptor = currentSendBuffer;
    currentSendBuffer = (currentSendBuffer + 1) % 8;

    if(size > 1518)
        size = 1518;

    for(uint8_t* src = buffer + size -1, *dst = (uint8_t*)(sendBufferDesc[sendDescriptor].address + size - 1);
        src >= buffer;
        --src, --dst)
    {
        *dst = *src;
    }
    printf("SEND: ");
    for(int i = 14+20; i < (size>64?64:size); i++)
    {
        printfHex(buffer[i]);
        printf(" ");
    }
    sendBufferDesc[sendDescriptor].avail = 0;
    sendBufferDesc[sendDescriptor].flags2 = 0;
    sendBufferDesc[sendDescriptor].flags = 0x8300F000
            | ((uint16_t)((-size) & 0xFFF));

    registerAddressPort.Write(0);
    registerDataPort.Write(0x48);
    printf("End");
}

void amd_am79c973::receive()
{
    printf("AMD am79c973 DATA RECEIVED\n");

    for(; (recvBufferDesc[currentRecvBuffer].flags & 0x80000000) == 0;
    currentRecvBuffer = (currentRecvBuffer + 1) % 8)
    {
        if(!(recvBufferDesc[currentRecvBuffer].flags & 0x40000000)
        && (recvBufferDesc[currentRecvBuffer].flags & 0x03000000) == 0x03000000)
        {
            uint32_t size = recvBufferDesc[currentRecvBuffer].flags & 0xFFF;
            if(size > 64) // remove checksum
            {
                size -= 4;
            }
            uint8_t* buffer = (uint8_t*)(recvBufferDesc[currentRecvBuffer].address);


            for(int i = 0; i < size; ++i)
            {
                printfHex(buffer[i]);
                printf(" ");
            }

        }

        recvBufferDesc[currentRecvBuffer].flags2 = 0;
        recvBufferDesc[currentRecvBuffer].flags = 0x8000F7FF;
    }
}