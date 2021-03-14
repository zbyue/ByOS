//
// Created by zby on 3/2/21.
//

#ifndef BYOS_VGA_H
#define BYOS_VGA_H

#include "../common/types.h"
#include "../hardware/port.h"

namespace byos
{
    namespace drivers
    {
        class VideoGraphicsArray
        {
        protected:
            hardware::Port8Bit miscPort;
            hardware::Port8Bit crtcIndexPort;
            hardware::Port8Bit crtcDataPort;
            hardware::Port8Bit sequencerIndexPort;
            hardware::Port8Bit sequencerDataPort;
            hardware::Port8Bit graphicsControllerIndexPort;
            hardware::Port8Bit graphicsControllerDataPort;
            hardware::Port8Bit attributeControllerIndexPort;
            hardware::Port8Bit attributeControllerReadPort;
            hardware::Port8Bit attributeControllerWritePort;
            hardware::Port8Bit attributeControllerResetPort;

            void writeRegisters(common::uint8_t* registers);
            common::uint8_t* getFrameBufferSegment();


            virtual common::uint8_t getColorIndex(common::uint8_t r, common::uint8_t g, common::uint8_t b);
        public:
            VideoGraphicsArray();
            ~VideoGraphicsArray();

            virtual bool supportsMode(common::uint32_t width, common::uint32_t height, common::uint32_t colordepth);
            virtual bool setMode(common::uint32_t width, common::uint32_t height, common::uint32_t colordepth);

            virtual void putPixel(common::int32_t x, common::int32_t y, common::uint8_t r, common::uint8_t g, common::uint8_t b);
            virtual void putPixel(common::int32_t x, common::int32_t y, common::uint8_t colorIndex);

            virtual void fillRectangle(common::uint32_t x, common::uint32_t y, common::uint32_t w, common::uint32_t h, common::uint8_t r, common::uint8_t g, common::uint8_t b);
        };
    }
}
#endif //BYOS_VGA_H
