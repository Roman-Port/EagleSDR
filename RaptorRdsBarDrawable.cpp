#include "RaptorRdsBarDrawable.h"
#include "RaptorFontStore.h"
#include <algorithm>

#define RAPTOR_RDSBAR_PADDING_V 10
#define RAPTOR_RDSBAR_PADDING_H 15
#define RAPTOR_RDSBAR_BACKGROUND raptor_pixel(0, 0, 0)
#define RAPTOR_RDSBAR_BORDER raptor_pixel(130, 130, 130)
#define RAPTOR_RDSBAR_FOREGROUND raptor_pixel(255, 255, 255)

RaptorRdsBarDrawable::RaptorRdsBarDrawable() {
	renderer.configure(&SYSTEM_REGULAR_15, RAPTOR_RDSBAR_FOREGROUND, RAPTOR_RDSBAR_BACKGROUND);
	renderer.character_spacing = 0;
	renderer.monospace = true;
	set_background_color(RAPTOR_RDSBAR_BACKGROUND);
}

int RaptorRdsBarDrawable::get_requested_height()
{
	return RAPTOR_RDSBAR_PADDING_V + RAPTOR_RDSBAR_PADDING_V + SYSTEM_REGULAR_15.height;
}

int RaptorRdsBarDrawable::get_requested_width()
{
	return RAPTOR_SCREEN_WIDTH;
}

void RaptorRdsBarDrawable::layout_self()
{
	//Layout left side
	int left = RAPTOR_RDSBAR_PADDING_H;
	pixelPs = get_pixel(left, RAPTOR_RDSBAR_PADDING_V);
	left += renderer.measure_line(8) + RAPTOR_RDSBAR_PADDING_H;
	pixelDividerX = left;
	left += 1 + RAPTOR_RDSBAR_PADDING_H;

	//Layout right side
	int right = RAPTOR_RDSBAR_PADDING_H;
	//...

	//Layout the RT bar
	pixelRt = get_pixel(left, RAPTOR_RDSBAR_PADDING_V);
	rtWidth = get_width() - left - right;
	rtChars = std::min(rtWidth / renderer.measure_line(1), 64);
}

void RaptorRdsBarDrawable::tick_self()
{
}

bool RaptorRdsBarDrawable::get_resize_allowed() {
	return false;
}

void RaptorRdsBarDrawable::draw_self()
{
	renderer.draw_line(pixelPs, "KQRS    ", 8);
	fill(RAPTOR_RDSBAR_BORDER, pixelDividerX, RAPTOR_RDSBAR_PADDING_V, 1, get_height() - RAPTOR_RDSBAR_PADDING_V - RAPTOR_RDSBAR_PADDING_V);
	renderer.draw_line(pixelRt, "Minnesota's Classic Rock Station 92.5 FM                                                                ", rtChars);
}