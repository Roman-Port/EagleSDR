#include "RaptorSystemStatusBarDrawable.h"
#include "RaptorSystemBarController.h"

#define RAPTOR_SYSBAR_PADDING 5
#define RAPTOR_SYSBAR_HEIGHT 20

RaptorSystemStatusBarDrawable::RaptorSystemStatusBarDrawable() {
	set_background_color(raptor_pixel(255, 255, 255));
}

int RaptorSystemStatusBarDrawable::get_requested_height() {
	return RAPTOR_SYSBAR_PADDING + RAPTOR_SYSBAR_HEIGHT + RAPTOR_SYSBAR_PADDING;
}

bool RaptorSystemStatusBarDrawable::get_resize_allowed() {
	return false;
}

void RaptorSystemStatusBarDrawable::layout_self()
{
}

void RaptorSystemStatusBarDrawable::tick_self()
{
}

void RaptorSystemStatusBarDrawable::draw_self()
{
	RaptorSystemBarController::draw(get_pixel(0, 0), RAPTOR_SYSBAR_PADDING, RAPTOR_SYSBAR_HEIGHT);
	invalidate();
}