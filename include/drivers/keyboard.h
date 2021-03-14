#ifndef __BYOS__DRIVERS__KEYBOARD_H
#define __BYOS__DRIVERS__KEYBOARD_H

#include "../common/types.h"
#include "../hardware/interrupts.h"
#include "../hardware/port.h"
#include "../drivers/driver.h"
#include "../common/io.h"

namespace byos
{
    namespace drivers
    {
        class KeyboardEventHandler
        {
        public:
            KeyboardEventHandler();

            virtual void onKeyDown(char);
            virtual void onKeyUp(char);
        };

        class KeyboardDriver : public hardware::InterruptHandler, public Driver 
        {
        public:
            KeyboardDriver(hardware::InterruptManager* manager, KeyboardEventHandler* handler);
            ~KeyboardDriver();

            virtual void Activate() override;
            virtual common::uint32_t HandleInterrupt(common::uint32_t esp);

        private:
            byos::hardware::Port8Bit dataport;
            byos::hardware::Port8Bit commandport;

            KeyboardEventHandler* handler;
        };

        class PrintKeyboardEventHandler : public KeyboardEventHandler
        {
        public:
            virtual void onKeyDown(char c)
            {
                char* foo = " ";
                foo[0] = c;
                byos::common::printf(foo);
            }

        };
    }
}

#endif
