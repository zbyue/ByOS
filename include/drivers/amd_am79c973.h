//
// Created by zbyue on 3/29/21.
//

#ifndef BYOS_AMD_AM79C973_H
#define BYOS_AMD_AM79C973_H

#include "../common/types.h"
#include "driver.h"
#include "../hardware/pci.h"
#include "../hardware/port.h"
#include "../hardware/interrupts.h"

namespace byos
{
    namespace drivers
    {
        class amd_am79c973 : public Driver, public hardware::InterruptHandler
        {
            struct InitializationBlock
            {
                common::uint16_t mode;
                unsigned reserved1 : 4;
                unsigned numSendBuffers : 4;
                unsigned reserved2 : 4;
                unsigned numRecvBuffers : 4;
                common::uint64_t physicalAddress : 48;
                common::uint16_t reserved3;
                common::uint64_t logicalAddress;
                common::uint32_t recvBufferDescAddress;
                common::uint32_t sendBufferDescAddress;
            } __attribute__((packed));
            struct BufferDescriptor
            {
                common::uint32_t address;
                common::uint32_t flags;
                common::uint32_t flags2;
                common::uint32_t avail;
            } __attribute__((packed));

            hardware::Port16Bit MACAddress0Port;
            hardware::Port16Bit MACAddress2Port;
            hardware::Port16Bit MACAddress4Port;
            hardware::Port16Bit registerDataPort;
            hardware::Port16Bit registerAddressPort;
            hardware::Port16Bit resetPort;
            hardware::Port16Bit busControlRegisterDataPort;

            InitializationBlock initBlock;

            BufferDescriptor* sendBufferDesc;
            common::uint8_t sendBufferDescMemory[2048 + 15];
            common::uint8_t sendBuffers[2 * 1024 + 15][8];
            common::uint8_t currentSendBuffer;

            BufferDescriptor* recvBufferDesc;
            common::uint8_t recvBufferDescMemory[2048 + 15];
            common::uint8_t recvBuffers[2 * 1024 + 15][8];
            common::uint8_t currentRecvBuffer;
        public:
            amd_am79c973(hardware::PciDeviceDescriptor *dev, hardware::InterruptManager* interruptManager);
            ~amd_am79c973();

            void Activate();
            int Reset();

            common::uint32_t HandleInterrupt(common::uint32_t esp);

            void send(common::uint8_t* buffer, int size);
            void receive();

        };
    }
}

#endif //BYOS_AMD_AM79C973_H
