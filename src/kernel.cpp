#include <common/types.h>
#include <gdt.h>
#include <hardware/interrupts.h>
#include <hardware/pci.h>
#include <common/io.h>
#include <drivers/amd_am79c973.h>
#include <drivers/ata.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/vga.h>
#include <gui/desktop.h>
#include <gui/window.h>
#include <multitasking.h>
#include <memorymanagement.h>

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

    uint32_t * memupper = (uint32_t*)((size_t)multiboot_structure + 8);
    size_t heap = 10 * 1024 * 1024;
    MemoryManager memoryManager(heap, (*memupper)*1024 - heap - 10*1024);

    printf("the size of MemoryChunk:");
    printfHex(sizeof (MemoryChunk));
    //printf("\n");
    printf("the size of int:");
    printfHex(sizeof (int));
    //printf("\n");
    printf("heap: 0x");
    printfHex((heap >> 24) & 0xFF);
    printfHex((heap >> 16) & 0xFF);
    printfHex((heap >> 8) & 0xFF);
    printfHex((heap) & 0xFF);

    void *allocated = memoryManager.malloc(1024);
    printf("\nallocated: 0x");
    printfHex(((size_t)allocated >> 24) & 0xFF);
    printfHex(((size_t)allocated >> 16) & 0xFF);
    printfHex(((size_t)allocated >> 8) & 0xFF);
    printfHex(((size_t)allocated) & 0xFF);
    printf("\n");
    /*
    Task task1(&gdt, taskA);
    Task task2(&gdt, taskB);
    Task task3(&gdt, taskC);
    taskManager.addTask(&task1);
    taskManager.addTask(&task2);
    taskManager.addTask(&task3);
    */

    InterruptManager interrupts(0x20, &gdt, &taskManager);
    
    // printf("Initializing Hardware Stage 1 \n");



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

    ///printf("Initializing Hardware Stage 2 \n");
    drvManager.ActivateAll();
    // printf("Initializing Hardware Stage 3 \n");

    //  interrupt 14
    AdvancedTechnologyAttachment ata0m(0x1F0, true);
    printf("ATA primary Master: ");
    ata0m.identify();
    AdvancedTechnologyAttachment ata0s(0x1F0, false);
    printf("ATA primary Slave: ");
    ata0s.identify();

    char* ataBuffer = "this is the content written into hard drive";
    ata0s.write28(0, (uint8_t*)ataBuffer, 43);
    ata0s.flush();

    ata0s.read28(0, (uint8_t*)ataBuffer, 43);
    //  interrupt 15
    AdvancedTechnologyAttachment ata1m(0x170, true);
    AdvancedTechnologyAttachment ata1s(0x170, false);

    // third: 0x1E8
    // fourth: 0x168

    interrupts.Activate();
    /*
    amd_am79c973* eth0 = (amd_am79c973*)(drvManager.drivers[2]);
    eth0->send((uint8_t*)"Hello Network", 13);
    */

    while(1)
    {
#ifdef GRAPHICSMODE
        desktop.draw(&vga);
#endif
    }
}

