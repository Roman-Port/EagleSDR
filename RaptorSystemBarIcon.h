#pragma once
#include "RaptorPixel.h"

class RaptorSystemBarIcon {

public:
	/// <summary>
	/// Updates the graphics and returns the offset width.
	/// </summary>
	/// <param name="ptr"></param>
	/// <returns></returns>
	virtual int update(raptor_pixel* ptr, int height) = 0;

};