#ifndef __BYOS__COMMON__IO_H
#define __BYOS__COMMON__IO_H

#include "types.h"


namespace byos
{
    namespace common
    {
        inline void printf(int8_t* str)
        {
            static uint16_t* VideoMemory = (uint16_t*)0xb8000;
            
            static uint8_t x = 0, y = 0;

            for(int i = 0; str[i] != '\0'; ++i)
            {
               switch(str[i])
               {
               case '\n':
                   y++;
                   x = 0;
                   break;
               default:
                   VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | str[i];
                   x++;
                   break;
               }

               if(x >= 80)
               {
                   y++;
                   x = 0;
               }

               if(y >= 25)
               {
                   for(y=0; y < 25; ++y)
                   {
                       for(x=0; x < 80; ++x)
                       {
                            VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | ' ';
                       }
                   }
                   x = 0;
                   y = 0;
               }
            }
        }

        inline void printfHex(uint8_t key)
        {
            char* foo = "00";
            char* hex = "0123456789ABCDEF";
            foo[0] = hex[(key >> 4) & 0x0F];
            foo[1] = hex[key & 0x0F];
            printf(foo);
         
        }
    }
}
#endif
