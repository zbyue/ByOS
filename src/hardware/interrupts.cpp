
#include "../../include/hardware/interrupts.h"
#include "../../include/common/io.h" 

using namespace byos::common;
using namespace byos::hardware;

InterruptManager::GateDescriptor InterruptManager::interruptDescriptorTable[256];

InterruptManager* InterruptManager::ActivateInterruptManager = 0;

InterruptHandler::InterruptHandler(uint8_t interruptNumber, InterruptManager* interruptManager)
{
    this->interruptNumber = interruptNumber;
    this->interruptManager = interruptManager;
    interruptManager->handlers[interruptNumber] = this;
}

InterruptHandler::~InterruptHandler()
{
    if(interruptManager->handlers[interruptNumber] == this)
    {
        interruptManager->handlers[interruptNumber] = 0;
    }
}

uint32_t InterruptHandler::HandleInterrupt(uint32_t esp)
{
    return esp;
}

void InterruptManager::setInterruptDescriptorTableEntry(
        uint8_t InterruptNumber,
        uint16_t codeSegmentSelectorOffset,
        void (*handler)(),
        uint8_t DescriptorPrivilegeLevel,
        uint8_t DescriptorType)
{
   const uint8_t IDT_DESC_PRESENT = 0x80;

   interruptDescriptorTable[InterruptNumber].handlerAddressLowBits = ((uint32_t)handler) & 0xFFFF;
   interruptDescriptorTable[InterruptNumber].handlerAddressHighBits = (((uint32_t)handler) >> 16) & 0xFFFF;
   interruptDescriptorTable[InterruptNumber].gdt_codeSegmentSelector = codeSegmentSelectorOffset;
   interruptDescriptorTable[InterruptNumber].access = IDT_DESC_PRESENT | DescriptorType | ((DescriptorPrivilegeLevel&3) << 5);
   interruptDescriptorTable[InterruptNumber].reserved = 0;
}

InterruptManager::InterruptManager(uint16_t hardwareInterruptOffset, GlobalDescriptorTable* gdt)
: picMasterCommand(0x20),
    picMasterData(0x21),
    picSlaveCommand(0xA0),
    picSlaveData(0xA1)
{
    this->hardwareInterruptOffset = hardwareInterruptOffset;
    uint16_t CodeSegment = gdt->CodeSegmentSelector();

    const uint8_t IDT_INTERRUPT_GATE = 0xE;

    for(uint8_t i = 255; i > 0; --i)
    {
        handlers[i] = 0;
        setInterruptDescriptorTableEntry(i, CodeSegment, &IgnoreInterrupt, 0, IDT_INTERRUPT_GATE);
    }
    handlers[0] = 0;
    setInterruptDescriptorTableEntry(0, CodeSegment, &IgnoreInterrupt, 0, IDT_INTERRUPT_GATE);
    
    setInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x00, CodeSegment, &HandleInterruptRequest0x00, 0, IDT_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x01, CodeSegment, &HandleInterruptRequest0x01, 0, IDT_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x0C, CodeSegment, &HandleInterruptRequest0x0C, 0, IDT_INTERRUPT_GATE);
    
    picMasterCommand.Write(0x11);
    picSlaveCommand.Write(0x11);

    picMasterData.Write(0x20);
    picSlaveData.Write(0x28);
 
    picMasterData.Write(0x04);
    picSlaveData.Write(0x02);

    picMasterData.Write(0x01);
    picSlaveData.Write(0x01);

    picMasterData.Write(0x00);
    picSlaveData.Write(0x00);

    InterruptDescriptorTablePointer idt;
    idt.size = 256 * sizeof(GateDescriptor) - 1;
    idt.base = (uint32_t)interruptDescriptorTable;
    
    asm volatile("lidt %0" : : "m" (idt));
}

void InterruptManager::Activate()
{
    if(ActivateInterruptManager != this)
    {
        ActivateInterruptManager->Deactivate();
    }
    ActivateInterruptManager = this;
    asm("sti");
}

void InterruptManager::Deactivate()
{
    if(ActivateInterruptManager == this)
    {
        ActivateInterruptManager = 0;
        asm("cli");
    }
}

InterruptManager::~InterruptManager()
{
    Deactivate();
}

uint32_t InterruptManager::HandleInterrupt(uint8_t InterruptNumber, uint32_t esp)
{
    if(ActivateInterruptManager != 0)
    {
        ActivateInterruptManager->DoHandleInterrupt(InterruptNumber, esp);
    }
    return esp;
}

uint32_t InterruptManager::DoHandleInterrupt(uint8_t InterruptNumber, uint32_t esp)
{
    if(handlers[InterruptNumber] != 0)
    {
        handlers[InterruptNumber]->HandleInterrupt(esp);
    }
    else if(InterruptNumber != 0x20)
    {
        printf("UNHANDLED INTERRUPT 0x");
        printfHex(InterruptNumber);
    }
    
    if(0x20 <= InterruptNumber && InterruptNumber < 0x30)
    {
        picMasterCommand.Write(0x20);
        if(InterruptNumber >= 0x28)
        {
            picSlaveCommand.Write(0x20);
        }
    }
    
    return esp;
}

uint16_t InterruptManager::HardwareInterruptOffset()
{
    return hardwareInterruptOffset;
}

