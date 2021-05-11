//
// Created by zbyue on 5/10/21.
//

#include "../../include/drivers/ata.h"
#include "../../include/common/io.h"
using namespace byos;
using namespace byos::common;
using namespace byos::drivers;
using namespace byos::hardware;



AdvancedTechnologyAttachment::AdvancedTechnologyAttachment(common::uint16_t portBase, bool master)
:   dataPort(portBase),
    errorPort(portBase + 1),
    sectorCountPort(portBase + 2),
    lbaLowPort(portBase + 3),
    lbaMidPort(portBase + 4),
    lbaHiPort(portBase + 5),
    devicePort(portBase + 6),
    commandPort(portBase + 7),
    controlPort(portBase + 0x206)
{
    bytesPerSector = 512;
    this->master = master;
}
AdvancedTechnologyAttachment::~AdvancedTechnologyAttachment()
{

}

void AdvancedTechnologyAttachment::identify()
{
    devicePort.Write(master ? 0xA0 : 0xB0);
    controlPort.Write(0);

    //devicePort.Write(0xA0);
    devicePort.Write(master ? 0xA0 : 0xB0);
    uint8_t status = commandPort.Read();

    if(status == 0xFF)
    {
        printf("ATA identify statusFF \n");
        return ;
    }

    devicePort.Write(master ? 0xA0 : 0xB0);
    sectorCountPort.Write(0);
    lbaLowPort.Write(0);
    lbaMidPort.Write(0);
    lbaHiPort.Write(0);
    commandPort.Write(0xEC);

    status = commandPort.Read();
    if(status == 0x00)
    {
        printf("ATA identify status00 \n");
        return ;
    }
    //printf("ATA identify status \n");
    while(((status & 0x80) == 0x80)
    && ((status & 0x01) != 0x01))
        status = commandPort.Read();

    if(status & 0x01)
    {
        printf("ERROR : ATA identify");
        return ;
    }
    for(uint16_t i = 0; i < 256; ++i)
    {
        uint16_t data = dataPort.Read();
        char* foo = "  \0";
        foo[1] = (data >> 8) & 0x00FF;
        foo[0] = data & 0x00FF;
        printf(foo);
    }
}

void AdvancedTechnologyAttachment::read28(common::uint32_t sector, common::uint8_t* data, int count)
{
    if(sector & 0xF0000000)
    {
        return;
    }
    if(count > bytesPerSector || count <=0)
    {
        return;
    }
    devicePort.Write((master ? 0xE0 : 0xF0) | ((sector & 0x0F000000) >> 24));
    errorPort.Write(0);
    sectorCountPort.Write(1);
    lbaLowPort.Write((sector & 0x000000FF));
    lbaMidPort.Write((sector & 0x0000FF00) >> 8);
    lbaHiPort.Write((sector & 0x00FF0000) >> 16);
    commandPort.Write(0x20);

    uint8_t status = commandPort.Read();

    while(((status & 0x80) == 0x80)
          && ((status & 0x01) != 0x01))
        status = commandPort.Read();

    if(status & 0x01)
    {
        printf("ERROR : ATA identify");
        return ;
    }

    printf("Reading from ATA: ");
    for(uint16_t i = 0; i < count; i += 2)
    {
        uint16_t wdata = dataPort.Read();

        char* foo = "  \0";
        foo[1] = (wdata >> 8) & 0x00FF;
        foo[0] = wdata & 0x00FF;
        printf(foo);

        data[i] = wdata & 0x00FF;
        if(i+1 < count)
        {
            data[i+1] |= (wdata >> 8) & 0x00FF;
        }
    }

    for(uint16_t i = count + (count%2); i < bytesPerSector; i += 2)
    {
        dataPort.Read();
    }
}

void AdvancedTechnologyAttachment::write28(common::uint32_t sector, common::uint8_t* data, int count)
{
    if(sector & 0xF0000000)
    {
        return;
    }
    if(count > bytesPerSector || count <=0)
    {
        return;
    }
    devicePort.Write((master ? 0xE0 : 0xF0) | ((sector & 0x0F000000) >> 24));
    errorPort.Write(0);
    sectorCountPort.Write(1);
    lbaLowPort.Write((sector & 0x000000FF));
    lbaMidPort.Write((sector & 0x0000FF00) >> 8);
    lbaHiPort.Write((sector & 0x00FF0000) >> 16);
    commandPort.Write(0x30);


    printf("Writing to ATA: ");
    for(uint16_t i = 0; i < count; i += 2)
    {
        uint16_t wdata = data[i];
        if(i+1 < count)
        {
            wdata |= ((uint16_t)data[i+1]) << 8;
        }
        char* foo = "  \0";
        foo[0] = (wdata >> 8) & 0x00FF;
        foo[1] = wdata & 0x00FF;
        printf(foo);

        dataPort.Write(wdata);
    }

    for(uint16_t i = count + (count%2); i < bytesPerSector; i += 2)
    {
        dataPort.Write(0x0000);
    }
}

void AdvancedTechnologyAttachment::flush()
{
    devicePort.Write(master ? 0xE0 : 0xF0);

    commandPort.Write(0xE7);

    uint8_t status = commandPort.Read();

    //printf("ATA flush status \n");
    while(((status & 0x80) == 0x80)
          && ((status & 0x01) != 0x01))
        status = commandPort.Read();

    if(status & 0x01)
    {
        printf("ERROR : ATA flush");
        return ;
    }
}