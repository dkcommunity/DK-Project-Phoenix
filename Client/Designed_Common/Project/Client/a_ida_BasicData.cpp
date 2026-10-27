#include "Client_PCH.h"

/*-----------------------------------------------------------------------------

    BasicData.cpp

    Basic data class implementation.

    2000.7.29. KJTINC

-----------------------------------------------------------------------------*/

#include "BasicData.h"

//----------------------------------------------------------------------------
// Class Point
//----------------------------------------------------------------------------

Point::Point(int _x, int _y) : x(_x), y(_y)
{
}

Point::Point() : x(0), y(0)
{
}

Point::~Point()
{
    // 析构函数 - 如果需要清理资源可以在这里添加
}

void Point::Set(int _x, int _y)
{
    x = _x;
    y = _y;
}

void Point::operator=(int val)
{
    x = val;
    y = val;
}

//----------------------------------------------------------------------------
// Class Rect
//----------------------------------------------------------------------------

Rect::Rect(int _x, int _y, int _w, int _h) : x(_x), y(_y), w(_w), h(_h)
{
}

Rect::Rect() : x(0), y(0), w(0), h(0)
{
}

Rect::~Rect()
{
    // 析构函数 - 如果需要清理资源可以在这里添加
}

void Rect::XY(int _x, int _y)
{
    x = _x;
    y = _y;
}

void Rect::WH(int _w, int _h)
{
    w = _w;
    h = _h;
}

void Rect::Set(int _x, int _y, int _w, int _h)
{
    x = _x;
    y = _y;
    w = _w;
    h = _h;
}

void Rect::operator=(int val)
{
    x = val;
    y = val;
    w = val;
    h = val;
}

void Rect::operator=(Point& point)
{
    x = point.x;
    y = point.y;
    // w和h保持不变
}

void Rect::operator+=(Point& point)
{
    x += point.x;
    y += point.y;
    // w和h保持不变
}

bool Rect::IsInRect(int _x, int _y) const
{
    if (_x >= x && _x < x + w) {
        if (_y >= y && _y < y + h) {
            return true;
        }
    }
    return false;
}

bool Rect::IsInRect(const Point& point) const
{
    return IsInRect(point.x, point.y);
}

int Rect::Left() const
{
    return x;
}

int Rect::Right() const
{
    return x + w - 1;
}

int Rect::Up() const
{
    return y;
}

int Rect::Down() const
{
    return y + h - 1;
}