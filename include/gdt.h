#ifndef __BYOS__GDT_H
#define __BYOS__GDT_H

#include "common/types.h"

namespace byos
{
    class GlobalDescriptorTable
    {
    public:
        class SegmentDescriptor
        {
        private:
            common::uint16_t limit_lo;
            common::uint16_t base_lo;
            common::uint8_t base_hi;
            common::uint8_t type;
            common::uint8_t limit_hi;
            common::uint8_t base_vhi;
        public:
            SegmentDescriptor(common::uint32_t base, common::uint32_t limit, common::uint8_t type);
            common::uint32_t Base();
            common::uint32_t Limit();
        } __attribute__((packed));

        SegmentDescriptor nullSegmentSelector;
        SegmentDescriptor unusedSegmentSelector;
        SegmentDescriptor codeSegmentSelector;
        SegmentDescriptor dataSegmentSelector;

        GlobalDescriptorTable();
        ~GlobalDescriptorTable();

        common::uint16_t CodeSegmentSelector();
        common::uint16_t DataSegmentSelector();


    };
}

#endif
