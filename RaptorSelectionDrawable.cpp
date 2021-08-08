#include "RaptorSelectionDrawable.h"
#include "RaptorFontStore.h"
#include "RaptorGraphicsStore.h"
#include <cstring>
#include <algorithm>

#define RAPTOR_SELECT_PADDING 5

RaptorSelectionDrawable::RaptorSelectionDrawable(char** selections, int selectionsCount, raptor_selection_callback callback) {
	//Apply settings
	this->selections = selections;
	this->selectionsCount = selectionsCount;
	this->callback = callback;
	this->index = 0;

	//Configure background
	set_background_color(RAPTOR_COLOR_FORE);

	//Configure font
	renderer.configure(&SYSTEM_REGULAR_20, raptor_pixel(255, 255, 255), RAPTOR_COLOR_FORE);
}

int RaptorSelectionDrawable::get_requested_width() {
	//Find the longest string
	int maxStrLen = 0;
	for (int i = 0; i < selectionsCount; i++)
		maxStrLen = std::max(maxStrLen, renderer.measure_line(selections[i]));

	return RAPTOR_SELECT_PADDING + maxStrLen + RAPTOR_SELECT_PADDING + ICON_UP.width + RAPTOR_SELECT_PADDING;
}

int RaptorSelectionDrawable::get_requested_height() {
	return RAPTOR_SELECT_PADDING + SYSTEM_REGULAR_20.height + RAPTOR_SELECT_PADDING;
}

bool RaptorSelectionDrawable::get_resize_allowed() {
	return false;
}

bool RaptorSelectionDrawable::process_input(int action) {
	if (action == RAPTOR_INPUT_UP) {
		index--;
		if (index < 0)
			index += selectionsCount;
		invalidate();
		return true;
	}
	if (action == RAPTOR_INPUT_DOWN) {
		index++;
		index = index % selectionsCount;
		invalidate();
		return true;
	}
	return false;
}

bool RaptorSelectionDrawable::hover_enter_self(int initiatorAction) {
	invalidate();
	return true;
}

void RaptorSelectionDrawable::hover_leave_self() {
	invalidate();
}

void RaptorSelectionDrawable::layout_self()
{
	int iconX = get_width() - RAPTOR_SELECT_PADDING - ICON_UP.width;
	int iconYUp = ((get_height() + 1) / 2) - 2 - ICON_UP.height;
	int iconYDown = ((get_height() + 1) / 2) + 2;
	iconUpPos = get_pixel(iconX, iconYUp);
	iconDownPos = get_pixel(iconX, iconYDown);
}

void RaptorSelectionDrawable::tick_self()
{
}

void RaptorSelectionDrawable::draw_self()
{
	//Render text centered vertically but stickied left
	renderer.draw_line(get_pixel(RAPTOR_SELECT_PADDING, (get_height() - SYSTEM_REGULAR_20.height) / 2), selections[index]);

	//If this is currently selected, highlight
	if (get_is_hovering())
		helper_draw_outline(get_pixel(0, 0), RAPTOR_COLOR_SELECT, get_width(), get_height(), 1);

	//Draw up icon
	if (index == 0)
		ICON_UP_DARK.draw(iconUpPos);
	else
		ICON_UP.draw(iconUpPos);

	//Draw down icon
	if (index + 1 == selectionsCount)
		ICON_DOWN_DARK.draw(iconDownPos);
	else
		ICON_DOWN.draw(iconDownPos);
}