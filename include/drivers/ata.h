//
// Created by zbyue on 5/10/21.
//

#ifndef BYOS_ATA_H
#define BYOS_ATA_H

#include "../common/types.h"
#include "../hardware/port.h"

namespace byos
{
    namespace drivers
    {
        class AdvancedTechnologyAttachment
        {
        protected:
            hardware::Port16Bit dataPort;
            hardware::Port8Bit errorPort;
            hardware::Port8Bit sectorCountPort;
            hardware::Port8Bit lbaLowPort;
            hardware::Port8Bit lbaMidPort;
            hardware::Port8Bit lbaHiPort;
            hardware::Port8Bit devicePort;
            hardware::Port8Bit commandPort;
            hardware::Port8Bit controlPort;

            bool master;
            common::uint16_t bytesPerSector;
        public:
            AdvancedTechnologyAttachment(common::uint16_t portBase, bool master);
            ~AdvancedTechnologyAttachment();

            void identify();
            void read28(common::uint32_t sector, common::uint8_t* data, int count);
            void write28(common::uint32_t sector, common::uint8_t* data, int count);
            void flush();
        };
    }
}


#endif //BYOS_ATA_H
