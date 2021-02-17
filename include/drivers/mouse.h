#ifndef __BYOS__DRIVERS__MOUSE_H
#define __BYOS__DRIVERS__MOUSE_H

#include "../common/types.h"
#include "../hardware/interrupts.h"
#include "../hardware/port.h"
#include "../drivers/driver.h"


namespace byos
{
    namespace drivers
    {
        class MouseEventHandler
        {
        public:
            MouseEventHandler();
            
            virtual void OnActivate();
            virtual void OnMouseDown(common::uint8_t button);
            virtual void OnMouseUp(common::uint8_t button);
            virtual void OnMouseMove(int x, int y);
        };

        class MouseDriver : public hardware::InterruptHandler, public Driver 
        {
        public:
            MouseDriver(hardware::InterruptManager* manager, MouseEventHandler* handler);
            ~MouseDriver();
            
            virtual void Activate();
            virtual common::uint32_t HandleInterrupt(common::uint32_t esp);

        private:
            hardware::Port8Bit dataport;
            hardware::Port8Bit commandport;
            
            common::uint8_t buffer[3];
            common::uint8_t offset;
            common::uint8_t buttons;

            MouseEventHandler* handler;
        };

        class MouseToConsole : public MouseEventHandler
        {
            common::int8_t x, y;
        public:
            MouseToConsole()
            {
            }

            void OnMouseMove(int xoffset, int yoffset)
            {
                static common::uint16_t* VideoMemory = (common::uint16_t*)0xb8000;
                VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xF000) >> 4)
                                    | ((VideoMemory[80*y+x] & 0x0F00) << 4)
                                    | (VideoMemory[80*y+x] & 0x00FF);

                x += xoffset;
                if(x < 0)
                {
                    x = 0;
                }
                else if(x>=80)
                {
                    x = 79;
                }
                y -= yoffset;
                if(y < 0)
                {
                    y = 0;
                }
                else if(y >= 25)
                {
                    y = 24;
                }

                VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xF000) >> 4)
                                    | ((VideoMemory[80*y+x] & 0x0F00) << 4)
                                    | (VideoMemory[80*y+x] & 0x00FF);
         
            }
        };
    }
}
#endif
