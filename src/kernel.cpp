#include "../include/common/types.h"
#include "../include/gdt.h"
#include "../include/hardware/interrupts.h"
#include "../include/hardware/pci.h"
#include "../include/common/io.h"
#include "../include/drivers/driver.h"
#include "../include/drivers/keyboard.h"
#include "../include/drivers/mouse.h"
#include "../include/drivers/vga.h"
#include "../include/gui/desktop.h"
#include "../include/gui/window.h"
#include "../include/multitasking.h"

//#define GRAPHICSMODE

using namespace byos;
using namespace byos::common;
using namespace byos::drivers;
using namespace byos::hardware;
using namespace byos::gui;

void taskA()
{
    while(true)
        printf("AAAA");
}

void taskB()
{
    while(true)
        printf("BBBB");
}

void taskC()
{
    while(true)
        printf("CCCC");
}



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
    GlobalDescriptorTable gdt;
    TaskManager taskManager;

    Task task1(&gdt, taskA);
    Task task2(&gdt, taskB);
    Task task3(&gdt, taskC);
    taskManager.addTask(&task1);
    taskManager.addTask(&task2);
    taskManager.addTask(&task3);

    InterruptManager interrupts(0x20, &gdt, &taskManager);
    
    printf("Initializing Hardware Stage 1 \n");



    DriverManager drvManager;
    PrintKeyboardEventHandler kbhandler;
    MouseToConsole mtconsole;
#ifdef GRAPHICSMODE
    Desktop desktop(320,200,0x00,0x00,0xA8);
    VideoGraphicsArray vga;
    vga.setMode(320,200,8);
    //vga.fillRectangle(0,0,320,200,0xFF,0xFF,0xFF);
    Window win1(&desktop, 20, 20, 80, 80, 0xFF, 0xFF, 0xFF);
    Window win2(&desktop, 100, 100, 40, 40, 0x00, 0x00, 0x00);
    desktop.addChild(&win1);
    desktop.addChild(&win2);
#endif

    KeyboardDriver keyboard(&interrupts, &kbhandler);
    MouseDriver mouse(&interrupts, &mtconsole);
    drvManager.AddDriver(&keyboard);
    drvManager.AddDriver(&mouse);

    PciController pciController;
    pciController.selectDrivers(&drvManager, &interrupts);



    printf("Initializing Hardware Stage 2 \n");
    drvManager.ActivateAll();
    printf("Initializing Hardware Stage 3 \n");







    interrupts.Activate();

    while(1)
    {
#ifdef GRAPHICSMODE
        desktop.draw(&vga);
#endif
    }
}

