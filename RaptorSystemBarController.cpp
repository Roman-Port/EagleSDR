#include "RaptorSystemBarController.h"
#include <cstring>

namespace RaptorSystemBarController {

	RaptorSystemBarIcon* icons[RAPTOR_SYSBAR_MAX_ITEMS];
	int priorities[RAPTOR_SYSBAR_MAX_ITEMS];
	int iconCount;

	void init() {
		iconCount = 0;
	}

	void draw(raptor_pixel* ptr, int padding, int height) {
		//Fill with white
		memset(ptr, 255, (padding + height + padding) * RAPTOR_SCREEN_WIDTH * sizeof(raptor_pixel));

		//Offset by padding
		ptr += padding;
		ptr += RAPTOR_SCREEN_WIDTH * padding;

		//Loop through and draw all icons
		for (int i = 0; i < iconCount; i++) {
			//Draw
			int itemSize = icons[i]->update(ptr, height);

			//Offset by size
			ptr += itemSize;

			//If size was not zero, offset by padding
			ptr += 5;
		}
	}

	void add_icon(RaptorSystemBarIcon* icon, int priority) {
		//Search for where in the list we can insert this
		int index = 0;
		while (index < iconCount && priorities[index] > priority)
			index++;

		//Now that we've located where it fits in, move everything down a slot
		for (int i = RAPTOR_SYSBAR_MAX_ITEMS - 2; i >= index; i--) {
			icons[i + 1] = icons[i];
			priorities[i + 1] = priorities[i];
		}

		//Set
		icons[index] = icon;
		priorities[index] = priority;

		//Apply state
		iconCount++;
	}

}