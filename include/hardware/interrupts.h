#ifndef __BYOS__HARDWARE__INTERRUPTS_H
#define __BYOS__HARDWARE__INTERRUPTS_H

#include "../common/types.h"
#include "port.h"
#include "../gdt.h"
#include "../multitasking.h"

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
            TaskManager* taskManager;
            
            static InterruptManager* ActivateInterruptManager;
            common::uint16_t hardwareInterruptOffset;
            
            Port8BitSlow picMasterCommand;
            Port8BitSlow picMasterData;
            Port8BitSlow picSlaveCommand;
            Port8BitSlow picSlaveData;
            
        public:
            InterruptManager(common::uint16_t hardwareInterruptOffset, GlobalDescriptorTable* gdt, TaskManager* taskManager);
            ~InterruptManager();

            common::uint16_t HardwareInterruptOffset();
            void Activate();
            void Deactivate();

            static common::uint32_t HandleInterrupt(common::uint8_t InterruptNumber, common::uint32_t esp);
            common::uint32_t DoHandleInterrupt(common::uint8_t InterruptNumber, common::uint32_t esp);

            static void IgnoreInterrupt();

            static void HandleInterruptRequest0x00();
            static void HandleInterruptRequest0x01();
            static void HandleInterruptRequest0x02();
            static void HandleInterruptRequest0x03();
            static void HandleInterruptRequest0x04();
            static void HandleInterruptRequest0x05();
            static void HandleInterruptRequest0x06();
            static void HandleInterruptRequest0x07();
            static void HandleInterruptRequest0x08();
            static void HandleInterruptRequest0x09();
            static void HandleInterruptRequest0x0A();
            static void HandleInterruptRequest0x0B();
            static void HandleInterruptRequest0x0C();
            static void HandleInterruptRequest0x0D();
            static void HandleInterruptRequest0x0E();
            static void HandleInterruptRequest0x0F();
            static void HandleInterruptRequest0x31();

            static void HandleInterruptRequest0x80();

            static void HandleException0x00();
            static void HandleException0x01();
            static void HandleException0x02();
            static void HandleException0x03();
            static void HandleException0x04();
            static void HandleException0x05();
            static void HandleException0x06();
            static void HandleException0x07();
            static void HandleException0x08();
            static void HandleException0x09();
            static void HandleException0x0A();
            static void HandleException0x0B();
            static void HandleException0x0C();
            static void HandleException0x0D();
            static void HandleException0x0E();
            static void HandleException0x0F();
            static void HandleException0x10();
            static void HandleException0x11();
            static void HandleException0x12();
            static void HandleException0x13();

        };
    }
}

#endif
