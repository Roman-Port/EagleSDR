#include "RaptorSysIconSignal.h"
#include "RaptorGraphicsStore.h"
#include <cstring>

#define RAPTOR_BAR_SPACING 5
#define RAPTOR_BAR_WIDTH 4

int RaptorSysIconSignal::update(raptor_pixel* ptr, int height)
{
	//Draw main icon
	ICON_SIGNAL_TOWER.draw(ptr);

	//Offset to the icon
	int offset = 7;
	ptr += offset;

	//Draw the line down from it
	for (int y = 0; y < height; y++)
		memset(ptr + (y * RAPTOR_SCREEN_WIDTH), 0, RAPTOR_BAR_WIDTH * sizeof(raptor_pixel));

	//Draw signal bars
	const int BAR_HEIGHTS[] = { 4, 6, 10, 15, 20 };
	for (int i = 0; i < 5; i++) {
		//Update state
		ptr += RAPTOR_BAR_SPACING;
		offset += RAPTOR_BAR_SPACING;

		//Determine the offset from the top
		int topOffset = height - BAR_HEIGHTS[i];

		//Draw
		while(topOffset < height)
			memset(ptr + (topOffset++ * RAPTOR_SCREEN_WIDTH), 0, RAPTOR_BAR_WIDTH * sizeof(raptor_pixel));
	}

	return offset + RAPTOR_BAR_WIDTH;
}
