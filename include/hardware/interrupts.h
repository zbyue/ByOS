#ifndef __BYOS__HARDWARE__INTERRUPTS_H
#define __BYOS__HARDWARE__INTERRUPTS_H

#include "../common/types.h"
#include "port.h"
#include "../gdt.h"

namespace byos
{
    namespace hardware
    {
        class InterruptManager;

        class InterruptHandler
        {
        protected:
            common::uint8_t interruptNumber;
            InterruptManager* interruptManager;

            InterruptHandler(common::uint8_t interruptNumber, InterruptManager* interruptManager);
            ~InterruptHandler();
        public:
            virtual common::uint32_t HandleInterrupt(common::uint32_t esp);
        };

        class InterruptManager
        {
            friend class InterruptHandler;
        protected:
            // the entry of idt
            struct GateDescriptor
            {
                common::uint16_t handlerAddressLowBits;
                common::uint16_t gdt_codeSegmentSelector;
                common::uint8_t reserved;
                common::uint8_t access;
                common::uint16_t handlerAddressHighBits;
            } __attribute__((packed));

            struct InterruptDescriptorTablePointer
            {
                common::uint16_t size;
                common::uint32_t base;
            } __attribute__((packed));

            static void setInterruptDescriptorTableEntry(
                common::uint8_t InterruptNumber,
                common::uint16_t codeSegmentSelectorOffset,
                void (*handler)(),
                common::uint8_t DescriptorPrivilegeLevel,
                common::uint8_t DescriptorType
            );
            
            static GateDescriptor interruptDescriptorTable[256];
            InterruptHandler* handlers[256];
            
            static InterruptManager* ActivateInterruptManager;
            common::uint16_t hardwareInterruptOffset;
            
            Port8BitSlow picMasterCommand;
            Port8BitSlow picMasterData;
            Port8BitSlow picSlaveCommand;
            Port8BitSlow picSlaveData;
            
        public:
            InterruptManager(common::uint16_t hardwareInterruptOffset, GlobalDescriptorTable* gdt);
            ~InterruptManager();

            common::uint16_t HardwareInterruptOffset();
            void Activate();
            void Deactivate();

            static common::uint32_t HandleInterrupt(common::uint8_t InterruptNumber, common::uint32_t esp);
            common::uint32_t DoHandleInterrupt(common::uint8_t InterruptNumber, common::uint32_t esp);

            static void IgnoreInterrupt();
            static void HandleInterruptRequest0x00();
            static void HandleInterruptRequest0x01();
            static void HandleInterruptRequest0x0C();

        };
    }
}

#endif
