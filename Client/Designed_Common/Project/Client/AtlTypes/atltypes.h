// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

// VS 6.0인 Darkeden 프로젝트에서도 사용가능하게 바꿈 by chyaya

#ifndef __ATLTYPES_H__
#define __ATLTYPES_H__

#pragma once
#include <windows.h>
/////////////////////////////////////////////////////////////////////////////
// Classes declared in this file

class CSize;
class CPoint;
class CRect;

/////////////////////////////////////////////////////////////////////////////
// CSize - An extent, similar to Windows SIZE structure.

class CSize : public tagSIZE {
public:
  // Constructors
  // construct an uninitialized size
  CSize();
  // create from two integers
  CSize(int initCX, int initCY);
  // create from another size
  CSize(SIZE initSize);
  // create from a point
  CSize(POINT initPt);
  // create from a DWORD: cx = LOWORD(dw) cy = HIWORD(dw)
  CSize(DWORD dwSize);

  // Operations
  BOOL operator==(SIZE size) const;
  BOOL operator!=(SIZE size) const;
  void operator+=(SIZE size);
  void operator-=(SIZE size);
  void SetSize(int CX, int CY);

  // Operators returning CSize values
  CSize operator+(SIZE size) const;
  CSize operator-(SIZE size) const;
  CSize operator-() const;

  // Operators returning CPoint values
  CPoint operator+(POINT point) const;
  CPoint operator-(POINT point) const;

  // Operators returning CRect values
  CRect operator+(const RECT *lpRect) const;
  CRect operator-(const RECT *lpRect) const;
};

/////////////////////////////////////////////////////////////////////////////
// CPoint - A 2-D point, similar to Windows POINT structure.

class CPoint : public tagPOINT {
public:
  // Constructors

  // create an uninitialized point
  CPoint();
  // create from two integers
  CPoint(int initX, int initY);
  // create from another point
  CPoint(POINT initPt);
  // create from a size
  CPoint(SIZE initSize);
  // create from an LPARAM: x = LOWORD(dw) y = HIWORD(dw)
  CPoint(LPARAM dwPoint);

  // Operations

  // translate the point
  void Offset(int xOffset, int yOffset);
  void Offset(POINT point);
  void Offset(SIZE size);
  void SetPoint(int X, int Y);

  BOOL operator==(POINT point) const;
  BOOL operator!=(POINT point) const;
  void operator+=(SIZE size);
  void operator-=(SIZE size);
  void operator+=(POINT point);
  void operator-=(POINT point);

  // Operators returning CPoint values
  CPoint operator+(SIZE size) const;
  CPoint operator-(SIZE size) const;
  CPoint operator-() const;
  CPoint operator+(POINT point) const;

  // Operators returning CSize values
  CSize operator-(POINT point) const;

  // Operators returning CRect values
  CRect operator+(const RECT *lpRect) const;
  CRect operator-(const RECT *lpRect) const;
};

/////////////////////////////////////////////////////////////////////////////
// CRect - A 2-D rectangle, similar to Windows RECT structure.

class CRect : public tagRECT {
  // Constructors
public:
  // uninitialized rectangle
  CRect();
  // from left, top, right, and bottom
  CRect(int l, int t, int r, int b);
  // copy constructor
  CRect(const RECT &srcRect);
  // from a pointer to another rect
  CRect(LPCRECT lpSrcRect);
  // from a point and size
  CRect(POINT point, SIZE size);
  // from two points
  CRect(POINT topLeft, POINT bottomRight);

  // Attributes (in addition to RECT members)

  // retrieves the width
  int Width() const;
  // returns the height
  int Height() const;
  // returns the size
  CSize Size() const;
  // reference to the top-left point
  CPoint &TopLeft();
  // reference to the bottom-right point
  CPoint &BottomRight();
  // const reference to the top-left point
  const CPoint &TopLeft() const;
  // const reference to the bottom-right point
  const CPoint &BottomRight() const;
  // the geometric center point of the rectangle
  CPoint CenterPoint() const;
  // swap the left and right
  void SwapLeftRight();
  static void WINAPI SwapLeftRight(LPRECT lpRect);

  // convert between CRect and LPRECT/LPCRECT (no need for &)
  operator LPRECT();
  operator LPCRECT() const;

  // returns TRUE if rectangle has no area
  BOOL IsRectEmpty() const;
  // returns TRUE if rectangle is at (0,0) and has no area
  BOOL IsRectNull() const;
  // returns TRUE if point is within rectangle
  BOOL PtInRect(POINT point) const;

  // Operations

  // set rectangle from left, top, right, and bottom
  void SetRect(int x1, int y1, int x2, int y2);
  void SetRect(POINT topLeft, POINT bottomRight);
  // empty the rectangle
  void SetRectEmpty();
  // copy from another rectangle
  void CopyRect(LPCRECT lpSrcRect);
  // TRUE if exactly the same as another rectangle
  BOOL EqualRect(LPCRECT lpRect) const;

  // Inflate rectangle's width and height by
  // x units to the left and right ends of the rectangle
  // and y units to the top and bottom.
  void InflateRect(int x, int y);
  // Inflate rectangle's width and height by
  // size.cx units to the left and right ends of the rectangle
  // and size.cy units to the top and bottom.
  void InflateRect(SIZE size);
  // Inflate rectangle's width and height by moving individual sides.
  // Left side is moved to the left, right side is moved to the right,
  // top is moved up and bottom is moved down.
  void InflateRect(LPCRECT lpRect);
  void InflateRect(int l, int t, int r, int b);

  // deflate the rectangle's width and height without
  // moving its top or left
  void DeflateRect(int x, int y);
  void DeflateRect(SIZE size);
  void DeflateRect(LPCRECT lpRect);
  void DeflateRect(int l, int t, int r, int b);

  // translate the rectangle by moving its top and left
  void OffsetRect(int x, int y);
  void OffsetRect(SIZE size);
  void OffsetRect(POINT point);
  void NormalizeRect();

  // absolute position of rectangle
  void MoveToY(int y);
  void MoveToX(int x);
  void MoveToXY(int x, int y);
  void MoveToXY(POINT point);

  // set this rectangle to intersection of two others
  BOOL IntersectRect(LPCRECT lpRect1, LPCRECT lpRect2);

  // set this rectangle to bounding union of two others
  BOOL UnionRect(LPCRECT lpRect1, LPCRECT lpRect2);

  // set this rectangle to minimum of two others
  BOOL SubtractRect(LPCRECT lpRectSrc1, LPCRECT lpRectSrc2);

  // Additional Operations
  void operator=(const RECT &srcRect);
  BOOL operator==(const RECT &rect) const;
  BOOL operator!=(const RECT &rect) const;
  void operator+=(POINT point);
  void operator+=(SIZE size);
  void operator+=(LPCRECT lpRect);
  void operator-=(POINT point);
  void operator-=(SIZE size);
  void operator-=(LPCRECT lpRect);
  void operator&=(const RECT &rect);
  void operator|=(const RECT &rect);

  // Operators returning CRect values
  CRect operator+(POINT point) const;
  CRect operator-(POINT point) const;
  CRect operator+(LPCRECT lpRect) const;
  CRect operator+(SIZE size) const;
  CRect operator-(SIZE size) const;
  CRect operator-(LPCRECT lpRect) const;
  CRect operator&(const RECT &rect2) const;
  CRect operator|(const RECT &rect2) const;
  CRect MulDiv(int nMultiplier, int nDivisor) const;
};

#ifndef _DEBUG
#define ATLTYPES_INLINE inline
#include "AtlTypes/atltypes.inl" //#include <atltypes.inl>
#endif

#endif // __ATLTYPES_H__
