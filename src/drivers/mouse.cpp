#include <drivers/mouse.h>
#include <common/io.h>

using namespace byos::common;
using namespace byos::drivers;
using namespace byos::hardware;

MouseEventHandler::MouseEventHandler()
{
}

void MouseEventHandler::onActivate()
{
}

void MouseEventHandler::onMouseDown(uint8_t button)
{
}

void MouseEventHandler::onMouseUp(uint8_t button)
{
}

void MouseEventHandler::onMouseMove(int x, int y)
{
}

MouseDriver::MouseDriver(InterruptManager* manager, MouseEventHandler* handler)
: InterruptHandler(0x2C, manager),
    dataport(0x60),
    commandport(0x64)
{
    this->handler = handler;
}

void MouseDriver::Activate()
{
    offset = 0;
    buttons = 0;

    commandport.Write(0xA8); // activate interrupt 
    commandport.Write(0x20); // get current state 
    uint8_t status = dataport.Read() | 2;
    commandport.Write(0x60); //set state
    dataport.Write(status);

    commandport.Write(0xD4);
    dataport.Write(0xF4);
    dataport.Read();
}

MouseDriver::~MouseDriver()
{
}

uint32_t MouseDriver::HandleInterrupt(uint32_t esp)
{
    uint8_t status = commandport.Read();
    if(!(status & 0x20))
    {
        return esp;
    }

    static int8_t x=40, y=12;
    buffer[offset]= dataport.Read();
    
    if(handler == 0)
    {
        return esp;
    }

    offset = (offset + 1) % 3;
    if(offset == 0)
    {
        if(buffer[1] != 0 || buffer[2] != 0)
        {
            handler->onMouseMove((int8_t)buffer[1], (int8_t)buffer[2]);
        }
        static uint16_t* VideoMemory = (uint16_t*)0xb8000;
        for(uint8_t i = 0; i < 3; i++)
        {
            if((buffer[0] & (0x01 << i)) != (buttons & (0x01 << i)))
            {
                // press buttons on mouse
                if(buttons & (0x01 << i))
                {
                    handler->onMouseUp(buttons); //release
                }
                else
                {
                    handler->onMouseDown(buttons); //press
                }
            }
       }
    }
    return esp;
}
