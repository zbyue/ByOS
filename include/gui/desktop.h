//
// Created by zby on 3/8/21.
//

#ifndef BYOS_DESKTOP_H
#define BYOS_DESKTOP_H

#include "widget.h"
#include "../drivers/mouse.h"

namespace byos
{
    namespace gui
    {
    class Desktop : public CompositeWidget
    {
    protected:
        common::uint32_t MouseX;
        common::uint32_t MouseY;

    public:
        Desktop(common::uint32_t w, common::uint32_t h, common::uint8_t r, common::uint8_t g, common::uint8_t b);
        ~Desktop();

        void draw(common::GraphicsContext* gc);
        void onMouseDown(common::uint8_t button);
        void onMouseUp(common::uint8_t button);
        void onMouseMove(common::int32_t x, common::int32_t y);
    };
    }
}
#endif //BYOS_DESKTOP_H
