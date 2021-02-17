#include "../../include/hardware/pci.h"
#include "../../include/drivers/driver.h"
#include "../../include/common/io.h"

using namespace byos::common;
using namespace byos::hardware;
using namespace byos::drivers;

PciDeviceDescriptor::PciDeviceDescriptor()
{

}

PciDeviceDescriptor::~PciDeviceDescriptor()
{

}

PciController::PciController()
    :dataPort(0xCFC),
    commandPort(0xCF8)
{

}

PciController::~PciController()
{
}

uint32_t PciController::Read(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset)
{
    uint32_t id = 
        0x1 << 31
        | ((bus & 0xFF) << 16)
        | ((device & 0x1F) << 11)
        | ((function & 0x07) << 8)
        | (registeroffset & 0xFC);
    commandPort.Write(id);
    uint32_t result = dataPort.Read();
    return result >> (8 * (registeroffset % 4));
}

void PciController::Write(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset, uint32_t value)
{
    uint32_t id = 
        0x1 << 31
        | ((bus & 0xFF) << 16)
        | ((device & 0x1F) << 11)
        | ((function & 0x07) << 8)
        | (registeroffset & 0xFC);
    commandPort.Write(id);
    dataPort.Write(value);
    
}
bool PciController::DeviceHasFunctions(uint16_t bus, uint16_t device)
{
    return Read(bus, device, 0, 0x0E) & (1<<7);
}

void PciController::selectDrivers(drivers::DriverManager *driverManager, hardware::InterruptManager* interruptManager)
{
    for(int bus = 0; bus < 8; ++bus)
    {
        for(int device = 0; device <32; ++device)
        {
            int numFunctions = DeviceHasFunctions(bus, device) ? 8 : 1;
            for(int function = 0; function < numFunctions; ++function)
            {
                PciDeviceDescriptor dev = getDeviceDescriptor(bus, device, function);

                if(dev.vendor_id == 0x0000 || dev.vendor_id == 0xFFFF)
                {
                    continue;
                }

                for(int barNum = 0; barNum < 6; ++barNum)
                {
                    BaseAddressRegister bar = getBaseAddressRegister(bus, device, function, barNum);
                    if(bar.address && (bar.type == InputOutput))
                    {
                        dev.portBase = (uint32_t)bar.address;
                    }

                    Driver* driver = getDriver(dev, interruptManager);
                    if(driver != 0)
                    {
                        driverManager->AddDriver(driver);
                    }
                }
                printf("PCI BUS: ");
                printfHex(bus & 0xFF);

                printf(", DEVICE: ");
                printfHex(device & 0xFF);

                printf(", FUNCTION: ");
                printfHex(function & 0xFF);

                printf(", vendor: ");
                printfHex((dev.vendor_id & 0xFF00) >> 8);
                printfHex(dev.vendor_id & 0xFF);
                printf(", device: ");
                printfHex((dev.device_id & 0xFF00) >> 8);
                printfHex(dev.device_id & 0xFF);
                printf("\n");
            }
        }
    }
}

PciDeviceDescriptor PciController::getDeviceDescriptor(common::uint16_t bus, common::uint16_t device, common::uint16_t function)
{
    PciDeviceDescriptor result;

    result.bus = bus;
    result.device = device;
    result.function = function;

    result.vendor_id = Read(bus, device, function, 0x00);
    result.device_id = Read(bus, device, function, 0x02);

    result.class_id = Read(bus, device, function, 0x0b);
    result.subclass_id = Read(bus, device, function, 0x0a);
    result.interface_id = Read(bus, device, function, 0x09);

    result.revision = Read(bus, device, function, 0x08);
    result.interrupt = Read(bus, device, function, 0x3c);

    return result;
}

Driver* PciController::getDriver(PciDeviceDescriptor dev,
                                 hardware::InterruptManager *interruptManager){
    switch (dev.vendor_id) {
        case 0x1022: //AMD
            switch (dev.device_id) {
                case 0x2000: //am79c973
                printf("AMD am79c973");
                    break;

            }
            break;
        case 0x8086: //Intel
            printf("Intel");
            break;

    }

    switch (dev.class_id) {
        case 0x03: //graphics
            switch (dev.subclass_id) {
                case 0x00: //VGA
                    break;

            }
            break;

    }
    return 0;
}

BaseAddressRegister PciController::getBaseAddressRegister(common::uint16_t bus, common::uint16_t device,
                                                          common::uint16_t function, common::uint16_t bar) {
    BaseAddressRegister result;

    uint32_t headertype = Read(bus, device, function, 0x0E) & 0x7F;
    int maxBARs = 6 - (4*headertype);
    if(bar >= maxBARs)
        return result;

    uint32_t bar_value = Read(bus, device, function, 0x10 + 4 * bar);
    result.type = (bar_value & 0x1) ? InputOutput : MemoryMapping;
    uint32_t temp;

    if(result.type == MemoryMapping)
    {
        // visit lowlevel.eu pci to compete
        switch ((bar_value >> 1) & 0x3) {
            case 0: //32 bit mode
            case 1: //20 bit mode
            case 2: //64 bit mode
                break;
        }
        result.prefetchable = ((bar_value >> 3) & 0x1) == 0x1;

    }
    else // InputOutput
    {
        result.address = (uint8_t*)(bar_value & ~0x3);
        result.prefetchable = false;
    }
    return result;
}
