#pragma once

#include "RaptorConstants.h"

struct raptor_pixel {
	
	unsigned char r;
	unsigned char g;
	unsigned char b;

	raptor_pixel() {
		r = 0;
		g = 0;
		b = 0;
	}

	raptor_pixel(int r, int g, int b) {
		this->r = r;
		this->g = g;
		this->b = b;
	}

	static raptor_pixel* get_offset_pixel(raptor_pixel* ctx, int offsetX, int offsetY) {
		return &ctx[offsetX + (RAPTOR_SCREEN_WIDTH * offsetY)];
	}
};