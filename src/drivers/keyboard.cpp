
#include <drivers/keyboard.h>
#include <common/io.h>

using namespace byos::drivers;
using namespace byos::hardware;
using namespace byos::common;

KeyboardEventHandler::KeyboardEventHandler()
{
}

void KeyboardEventHandler::onKeyDown(char) {
}

void KeyboardEventHandler::onKeyUp(char)
{
}

KeyboardDriver::KeyboardDriver(InterruptManager* manager, KeyboardEventHandler* handler)
: InterruptHandler(0x21, manager),
    dataport(0x60),
    commandport(0x64)
{
    this->handler = handler;
}

void KeyboardDriver::Activate()
{
    while(commandport.Read() & 0x1)
    {
        dataport.Read();
    }
    commandport.Write(0xAE); // activate interrupt 
    commandport.Write(0x20); // get current state 
    uint8_t status = (dataport.Read() | 1) & ~0x10;
    commandport.Write(0x60); //set state
    dataport.Write(status);

    dataport.Write(0xF4);
}

KeyboardDriver::~KeyboardDriver()
{
}

uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp)
{
    uint8_t key = dataport.Read();

    static bool Shift = false;
    
    switch(key)
    {
    case 0xFA: 
    case 0x45:
    case 0xC5:
               break;

    case 0x02: if(Shift) handler->onKeyDown('!'); else handler->onKeyDown('1'); break;
    case 0x03: if(Shift) handler->onKeyDown('@'); else handler->onKeyDown('2'); break;
    case 0x04: if(Shift) handler->onKeyDown('#'); else handler->onKeyDown('3'); break;
    case 0x05: if(Shift) handler->onKeyDown('$'); else handler->onKeyDown('4'); break;
    case 0x06: if(Shift) handler->onKeyDown('%'); else handler->onKeyDown('5'); break;
    case 0x07: if(Shift) handler->onKeyDown('^'); else handler->onKeyDown('6'); break;
    case 0x08: if(Shift) handler->onKeyDown('&'); else handler->onKeyDown('7'); break;
    case 0x09: if(Shift) handler->onKeyDown('*'); else handler->onKeyDown('8'); break;
    case 0x0A: if(Shift) handler->onKeyDown('('); else handler->onKeyDown('9'); break;
    case 0x0B: if(Shift) handler->onKeyDown(')'); else handler->onKeyDown('0'); break;
    case 0x0C: if(Shift) handler->onKeyDown('_'); else handler->onKeyDown('-'); break;
    case 0x0D: if(Shift) handler->onKeyDown('+'); else handler->onKeyDown('='); break;
    
    case 0x10: if(Shift) handler->onKeyDown('Q'); else handler->onKeyDown('q'); break;
    case 0x11: if(Shift) handler->onKeyDown('W'); else handler->onKeyDown('w'); break;
    case 0x12: if(Shift) handler->onKeyDown('E'); else handler->onKeyDown('e'); break;
    case 0x13: if(Shift) handler->onKeyDown('R'); else handler->onKeyDown('r'); break;
    case 0x14: if(Shift) handler->onKeyDown('T'); else handler->onKeyDown('t'); break;
    case 0x15: if(Shift) handler->onKeyDown('Y'); else handler->onKeyDown('y'); break;
    case 0x16: if(Shift) handler->onKeyDown('U'); else handler->onKeyDown('u'); break;
    case 0x17: if(Shift) handler->onKeyDown('I'); else handler->onKeyDown('i'); break;
    case 0x18: if(Shift) handler->onKeyDown('O'); else handler->onKeyDown('o'); break;
    case 0x19: if(Shift) handler->onKeyDown('P'); else handler->onKeyDown('p'); break;
    case 0x1A: if(Shift) handler->onKeyDown('{'); else handler->onKeyDown('['); break;
    case 0x1B: if(Shift) handler->onKeyDown('}'); else handler->onKeyDown(']'); break;
    case 0x2B: if(Shift) handler->onKeyDown('|'); else handler->onKeyDown('\\'); break;
    
    case 0x1E: if(Shift) handler->onKeyDown('A'); else handler->onKeyDown('a'); break;
    case 0x1F: if(Shift) handler->onKeyDown('S'); else handler->onKeyDown('s'); break;
    case 0x20: if(Shift) handler->onKeyDown('D'); else handler->onKeyDown('d'); break;
    case 0x21: if(Shift) handler->onKeyDown('F'); else handler->onKeyDown('f'); break;
    case 0x22: if(Shift) handler->onKeyDown('G'); else handler->onKeyDown('g'); break;
    case 0x23: if(Shift) handler->onKeyDown('H'); else handler->onKeyDown('h'); break;
    case 0x24: if(Shift) handler->onKeyDown('J'); else handler->onKeyDown('j'); break;
    case 0x25: if(Shift) handler->onKeyDown('K'); else handler->onKeyDown('k'); break;
    case 0x26: if(Shift) handler->onKeyDown('L'); else handler->onKeyDown('l'); break;
    case 0x27: if(Shift) handler->onKeyDown(':'); else handler->onKeyDown(';'); break;
    case 0x28: if(Shift) handler->onKeyDown('\"'); else handler->onKeyDown('\''); break;
    case 0x29: if(Shift) handler->onKeyDown('~'); else handler->onKeyDown('`'); break;
    
    case 0x2C: if(Shift) handler->onKeyDown('Z'); else handler->onKeyDown('z'); break;
    case 0x2D: if(Shift) handler->onKeyDown('X'); else handler->onKeyDown('x'); break;
    case 0x2E: if(Shift) handler->onKeyDown('C'); else handler->onKeyDown('c'); break;
    case 0x2F: if(Shift) handler->onKeyDown('V'); else handler->onKeyDown('v'); break;
    case 0x30: if(Shift) handler->onKeyDown('B'); else handler->onKeyDown('b'); break;
    case 0x31: if(Shift) handler->onKeyDown('N'); else handler->onKeyDown('n'); break;
    case 0x32: if(Shift) handler->onKeyDown('M'); else handler->onKeyDown('m'); break;
    case 0x33: if(Shift) handler->onKeyDown('<'); else handler->onKeyDown(','); break;
    case 0x34: if(Shift) handler->onKeyDown('>'); else handler->onKeyDown('.'); break;
    case 0x35: if(Shift) handler->onKeyDown('?'); else handler->onKeyDown('/'); break;

    case 0x1C: handler->onKeyDown('\n'); break;
    case 0x39: handler->onKeyDown(' '); break;
    case 0x2A: case 0x36: Shift = true; break;
    case 0xAA: case 0xB6: Shift = false; break;
    case 0x3A: if(Shift) Shift = false; else Shift = true; break;

    default:
        if(key < 0x80){
            printf("KEYBOARD 0x");
            printfHex(key);
        }
        break;
    }
    
    return esp;
}
