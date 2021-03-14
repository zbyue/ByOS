//
// Created by zby on 3/8/21.
//

#include "../../include/gui/window.h"

using namespace byos::common;
using namespace byos::gui;

Window::Window(Widget* parent, common::uint32_t x, common::uint32_t y, common::uint32_t w, common::uint32_t h, common::uint8_t r, common::uint8_t g, common::uint8_t b)
: CompositeWidget(parent, x, y, w, h, r, g, b)
{
    Dragging = false;
}
Window::~Window()
{

}

void Window::onMouseDown(common::uint32_t x, common::uint32_t y, common::uint8_t button)
{
    //Dragging = button == 1;
    Dragging = true;
    CompositeWidget::onMouseDown(x, y, button);
}
void Window::onMouseUp(common::uint32_t x, common::uint32_t y, common::uint8_t button)
{
    Dragging = false;
    CompositeWidget::onMouseUp(x, y, button);
}
void Window::onMouseMove(common::uint32_t oldx, common::uint32_t oldy, common::uint32_t newx, common::uint32_t newy)
{
    if(Dragging)
    {
        this->x += newx - oldx;
        this->y += newy - oldy;
    }
    CompositeWidget::onMouseMove(oldx, oldy, newx, newy);
}
