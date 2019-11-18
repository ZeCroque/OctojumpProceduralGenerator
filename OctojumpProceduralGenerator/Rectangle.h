#pragma once

#include <algorithm>

class Rectangle
{
	public :
		int _xOrigin;
		int _yOrigin;
		int _xEnd;
		int _yEnd;

		Rectangle() : _xOrigin(0), _xEnd(0), _yOrigin(0), _yEnd(0) {}
		Rectangle(int xOrigin, int yOrigin, int xEnd, int yEnd) : _xOrigin(xOrigin), _xEnd(xEnd), _yOrigin(yOrigin), _yEnd(yEnd) {}
		virtual ~Rectangle(){}
};