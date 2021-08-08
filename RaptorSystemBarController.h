#pragma once
#include "RaptorSystemBarIcon.h"

#define RAPTOR_SYSBAR_MAX_ITEMS 128

namespace RaptorSystemBarController {

	void init();
	void draw(raptor_pixel* ptr, int padding, int height);
	void add_icon(RaptorSystemBarIcon* icon, int priority);

}