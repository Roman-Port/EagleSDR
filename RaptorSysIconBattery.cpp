#include "RaptorSysIconBattery.h"
#include "RaptorGraphicsStore.h"

int RaptorSysIconBattery::update(raptor_pixel* ptr, int height)
{
	int padding = ((height - ICON_BATTERY_0.height) / 2) * RAPTOR_SCREEN_WIDTH;
	ICON_BATTERY_0.draw(ptr + padding);
	return ICON_BATTERY_0.width;
}
