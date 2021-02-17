#include "../include/common/types.h"
#include "../include/gdt.h"
#include "../include/hardware/interrupts.h"
#include "../include/hardware/pci.h"
#include "../include/common/io.h"
#include "../include/drivers/driver.h"
#include "../include/drivers/keyboard.h"
#include "../include/drivers/mouse.h"

using namespace byos;
using namespace byos::common;
using namespace byos::drivers;
using namespace byos::hardware;

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
    for(constructor* i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}

extern "C" void kernelMain(const void* multiboot_structure, uint32_t /*magicnumber*/)
{
    printf("Hello, this is myos!\n");

    GlobalDescriptorTable gdt;
    InterruptManager interrupts(0x20, &gdt);
    
    printf("Initializing Hardware Stage 1 \n");
    DriverManager drvManager;
    PrintKeyboardEventHandler kbhandler;
    MouseToConsole mtconsole;

    KeyboardDriver keyboard(&interrupts, &kbhandler);
    MouseDriver mouse(&interrupts, &mtconsole);
    drvManager.AddDriver(&keyboard);
    drvManager.AddDriver(&mouse);

    PciController pciController;
    pciController.selectDrivers(&drvManager, &interrupts);

    printf("Initializing Hardware Stage 2 \n");
    drvManager.ActivateAll();
    interrupts.Activate();

    printf("Initializing Hardware Stage 3 \n");
    while(1);
}

