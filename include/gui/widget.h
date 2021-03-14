//
// Created by zby on 3/8/21.
//

#ifndef BYOS_WIDGET_H
#define BYOS_WIDGET_H

#include "../common/types.h"
#include "../common/graphicscontext.h"
#include "../drivers/keyboard.h"
#include "../drivers/mouse.h"

namespace byos
{
    namespace gui
    {
    class Widget : public drivers::KeyboardEventHandler, public drivers::MouseEventHandler
        {
        protected:
            Widget* parent;
            common::uint32_t x;
            common::uint32_t y;
            common::uint32_t w;
            common::uint32_t h;

            common::uint8_t r;
            common::uint8_t g;
            common::uint8_t b;
            bool Focussable;

        public:
            Widget(Widget* parent,
            common::uint32_t x,
            common::uint32_t y,
            common::uint32_t w,
            common::uint32_t h,
            common::uint8_t r,
            common::uint8_t g,
            common::uint8_t b);
            ~Widget();
            virtual void getFocus(Widget* widget);
            virtual bool ContainsCoordinate(common::uint32_t x, common::uint32_t y);
            virtual void draw(common::GraphicsContext* gc);
            virtual void ModelToScreen(common::int32_t& x, common::int32_t& y);

            virtual void onMouseDown(common::uint32_t x, common::uint32_t y, common::uint8_t button);
            virtual void onMouseUp(common::uint32_t x, common::uint32_t y, common::uint8_t button);
            virtual void onMouseMove(common::uint32_t oldx, common::uint32_t oldy, common::uint32_t newx, common::uint32_t newy);

            virtual void onKeyDown(char);
            virtual void onKeyUp(char);
        };

        class CompositeWidget : public Widget
        {
        private:
            Widget* children[100];
            int numChildren;
            Widget* focussedChild;
        public:
            CompositeWidget(Widget* parent,
            common::uint32_t x,
                    common::uint32_t y,
            common::uint32_t w,
                    common::uint32_t h,
            common::uint8_t r,
                    common::uint8_t g,
            common::uint8_t b);
            ~CompositeWidget();
            virtual void getFocus(Widget* widget);
            virtual bool addChild(Widget* child);
            virtual void draw(common::GraphicsContext* gc);

            virtual void onMouseDown(common::int32_t x, common::int32_t y, common::uint8_t button);
            virtual void onMouseUp(common::int32_t x, common::int32_t y, common::uint8_t button);
            virtual void onMouseMove(common::int32_t oldx, common::int32_t oldy, common::int32_t newx, common::int32_t newy);

            virtual void onKeyDown(char);
            virtual void onKeyUp(char);
        };
    }
}

#endif //BYOS_WIDGET_H
