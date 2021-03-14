//
// Created by zby on 3/8/21.
//

#ifndef BYOS_WINDOW_H
#define BYOS_WINDOW_H

#include "widget.h"
#include "../drivers/mouse.h"

namespace byos
{
    namespace gui
    {
        class Window : public CompositeWidget
        {
        protected:
            bool Dragging;
        public:
            Window(Widget* parent,
                   common::uint32_t x,
                   common::uint32_t y,
                   common::uint32_t w,
                   common::uint32_t h,
                   common::uint8_t r,
                   common::uint8_t g,
                   common::uint8_t b);
            ~Window();

            void onMouseDown(common::uint32_t x, common::uint32_t y, common::uint8_t button);
            void onMouseUp(common::uint32_t x, common::uint32_t y, common::uint8_t button);
            void onMouseMove(common::uint32_t oldx, common::uint32_t oldy, common::uint32_t newx, common::uint32_t newy);
        };
    }
}
#endif //BYOS_WINDOW_H
