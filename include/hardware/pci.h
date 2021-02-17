#ifndef __BYOS__HARDWARE__PCI_H
#define __BYOS__HARDWARE__PCI_H

#include "port.h"
#include "../common/types.h"
#include "../drivers/driver.h"
#include "interrupts.h"

namespace byos
{
    namespace hardware
    {
        enum BaseAddressRegisterType
        {
            MemoryMapping = 0,
            InputOutput = 1
        };

        class BaseAddressRegister
        {
        public:
            bool prefetchable;
            common::uint8_t* address;
            common::uint32_t size;
            BaseAddressRegisterType type;
        };

        class PciDeviceDescriptor
        {
        public:
            PciDeviceDescriptor();
            ~PciDeviceDescriptor();
            common::uint32_t portBase;
            common::uint32_t interrupt;

            common::uint16_t bus;
            common::uint16_t device;
            common::uint16_t function;

            common::uint16_t vendor_id;
            common::uint16_t device_id;

            common::uint16_t class_id;
            common::uint16_t subclass_id;
            common::uint16_t interface_id;

            common::uint16_t revision;
        };
        class PciController
        {
            Port32Bit dataPort;
            Port32Bit commandPort;

        public:
            PciController();
            ~PciController();

            common::uint32_t Read(common::uint16_t bus, common::uint16_t device, common::uint16_t function, common::uint32_t registeroffset);
            void Write(common::uint16_t bus, common::uint16_t device, common::uint16_t function, common::uint32_t registeroffset, common::uint32_t value);
            bool DeviceHasFunctions(common::uint16_t bus, common::uint16_t device);

            void selectDrivers(drivers::DriverManager* driverManager, hardware::InterruptManager* interruptManager);
            PciDeviceDescriptor getDeviceDescriptor(common::uint16_t bus, common::uint16_t device, common::uint16_t function);
            BaseAddressRegister getBaseAddressRegister(common::uint16_t bus, common::uint16_t device, common::uint16_t function, common::uint16_t bar);
            drivers::Driver* getDriver(PciDeviceDescriptor dev, hardware::InterruptManager* interruptManager);
        };
    }
}


#endif
