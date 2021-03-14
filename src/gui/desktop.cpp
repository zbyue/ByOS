//
// Created by zby on 3/8/21.
//

#include "../../include/gui/desktop.h"

using namespace byos::common;
using namespace byos::gui;


Desktop::Desktop(uint32_t w, uint32_t h, uint8_t r, uint8_t g, uint8_t b)
: CompositeWidget(0, 0, 0, w, h, r, g, b)
{
    MouseX = w/2;
    MouseY = h/2;
}

Desktop::~Desktop()
{

}

void Desktop::draw(GraphicsContext* gc)
{
    CompositeWidget::draw(gc);

    for(int i = 0; i< 3; ++i)
    {
        gc -> putPixel(MouseX-i, MouseY, 0xFF, 0xFF,0xFF);
        gc -> putPixel(MouseX+i, MouseY, 0xFF, 0xFF,0xFF);
        gc -> putPixel(MouseX, MouseY-i, 0xFF, 0xFF,0xFF);
        gc -> putPixel(MouseX, MouseY+i, 0xFF, 0xFF,0xFF);
    }
}

void Desktop::onMouseDown(uint8_t button)
{
    CompositeWidget::onMouseDown(MouseX, MouseY, button);
}

void Desktop::onMouseUp(uint8_t button)
{
    CompositeWidget::onMouseUp(MouseX, MouseY, button);
}
void Desktop::onMouseMove(int32_t x, int32_t y)
{
    x /= 4;
    y /= 4;

    int32_t newMouseX = MouseX + x;
    if(newMouseX < 0)
        newMouseX = 0;
    if(newMouseX >= w)
        newMouseX = w-1;

    int32_t newMouseY = MouseY - y;
    if(newMouseY < 0)
        newMouseY = 0;
    if(newMouseY >= h)
        newMouseY = h-1;

    CompositeWidget::onMouseMove(MouseX, MouseY, newMouseX, newMouseY);

    MouseX = newMouseX;
    MouseY = newMouseY;
}
