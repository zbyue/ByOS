//
// Created by zby on 3/8/21.
//
#include "../../include/gui/widget.h"

using namespace byos::common;
using namespace byos::gui;

Widget::Widget(Widget* parent,
uint32_t x,
uint32_t y,
uint32_t w,
uint32_t h,
uint8_t r,
uint8_t g,
uint8_t b)
: KeyboardEventHandler()
{
    this->parent = parent;
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    this->r = r;
    this->g = g;
    this->b = b;
    this->Focussable = true;
}
Widget::~Widget()
{

}

bool Widget::ContainsCoordinate(common::uint32_t x, common::uint32_t y)
{
    return this->x <= x && x <= this->x + this->w
    && this->y <= y && y <= this->y + this->h;
}

void Widget::getFocus(Widget* widget)
{
    if(parent != 0)
    {
        parent->getFocus(widget);
    }
}

void Widget::draw(GraphicsContext* gc)
{
    int X = 0;
    int Y = 0;
    ModelToScreen(X, Y);
    gc->fillRectangle(X, Y, w, h, r, g, b);
}

void Widget::ModelToScreen(int32_t& x, int32_t& y)
{
    if(parent != 0)
    {
        parent->ModelToScreen(x, y);
    }
    x += this->x;
    y += this->y;
}

void Widget::onMouseDown(uint32_t x, uint32_t y, common::uint8_t button)
{
    if(Focussable)
    {
        getFocus(this);
    }

}
void Widget::onMouseUp(uint32_t x, uint32_t y, common::uint8_t button)
{

}
void Widget::onMouseMove(uint32_t oldx, uint32_t oldy, uint32_t newx, uint32_t newy)
{

}

void Widget::onKeyDown(char)
{

}
void Widget::onKeyUp(char)
{

}

CompositeWidget::CompositeWidget(Widget* parent,
uint32_t x,
uint32_t y,
uint32_t w,
uint32_t h,
uint8_t r,
uint8_t g,
uint8_t b)
:Widget(parent, x, y, w, h, r, g, b)
{
    this->focussedChild = 0;
    this->numChildren = 0;
}
CompositeWidget::~CompositeWidget()
{

}
void CompositeWidget::getFocus(Widget* widget)
{
    this->focussedChild = widget;
    if(parent != 0)
    {
        parent->getFocus(this);
    }
}

bool CompositeWidget::addChild(Widget* child)
{
    if(numChildren >= 100)
    {
        return false;
    }
    children[numChildren++] = child;
    return true;
}

void CompositeWidget::draw(GraphicsContext* gc)
{
    Widget::draw(gc);
    for(int i = numChildren-1; i >= 0; --i)
    {
        children[i]->draw(gc);
    }
}

void CompositeWidget::onMouseDown(int32_t x, int32_t y, uint8_t button)
{
    for(int i = 0; i < numChildren; ++i)
    {
        if(children[i]->ContainsCoordinate(x - this->x, y - this->y))
        {
            children[i]->onMouseDown(x - this->x, y - this->y, button);
            break;
        }
    }
}

void CompositeWidget::onMouseUp(int32_t x, int32_t y, uint8_t button)
{
    for(int i = 0; i < numChildren; ++i)
    {
        if(children[i]->ContainsCoordinate(x - this->x, y - this->y))
        {
            children[i]->onMouseUp(x - this->x, y - this->y, button);
            break;
        }
    }
}
void CompositeWidget::onMouseMove(int32_t oldx, int32_t oldy, int32_t newx, int32_t newy)
{
    int firstChild = -1;
    for(int i = 0; i < numChildren; ++i)
    {
        if(children[i]->ContainsCoordinate(oldx - this->x, oldy - this->y))
        {
            firstChild = i;
            children[i]->onMouseMove(oldx - this->x, oldy - this->y, newx - this->x, newy - this->y);
            break;
        }
    }
    for(int i = 0; i < numChildren; ++i)
    {
        if(children[i]->ContainsCoordinate(newx - this->x, newy - this->y))
        {
            if(firstChild != i)
                children[i]->onMouseMove(oldx - this->x, oldy - this->y, newx - this->x, newy - this->y);
            break;
        }
    }
}

void CompositeWidget::onKeyDown(char str)
{
    if(focussedChild != 0)
    {
        focussedChild->onKeyDown(str);
    }
}
void CompositeWidget::onKeyUp(char str)
{
    if(focussedChild != 0)
    {
        focussedChild->onKeyUp(str);
    }
}