#include "RaptorPaddedButtonDrawable.h"

RaptorPaddedButtonDrawable::RaptorPaddedButtonDrawable() {
	borderWidth = 1;
	padding = 10;
	border_color = RAPTOR_COLOR_BORDER;
	highlighted_color = RAPTOR_COLOR_SELECT;
	set_background_color(RAPTOR_COLOR_FORE);
}

int RaptorPaddedButtonDrawable::get_requested_height()
{
	//Start with default and then request it from each child
	int size = padding + padding + borderWidth + borderWidth;
	RaptorDrawable* cursor = 0;
	while (enumerate_children(&cursor))
		size += cursor->get_requested_height();
	return size;
}

int RaptorPaddedButtonDrawable::get_requested_width()
{
	//Start with default and then request it from each child
	int size = padding + padding + borderWidth + borderWidth;
	RaptorDrawable* cursor = 0;
	while (enumerate_children(&cursor))
		size += cursor->get_requested_width();
	return size;
}

void RaptorPaddedButtonDrawable::layout_self()
{
	//This is likely to only have one child, so just position all children in the middle
	int spacing = padding + borderWidth;
	RaptorDrawable* cursor = 0;
	while (enumerate_children(&cursor))
		cursor->request_layout(spacing, spacing, get_width() - spacing - spacing, get_height() - spacing - spacing);
}

void RaptorPaddedButtonDrawable::draw_self()
{
	draw_border(get_is_hovering());
}

void RaptorPaddedButtonDrawable::tick_self()
{
}

bool RaptorPaddedButtonDrawable::process_input(int action) {
	//Check for the enter action
	if (action == RAPTOR_INPUT_ENTER) {
		button_pressed();
		return true;
	}

	return false;
}

bool RaptorPaddedButtonDrawable::hover_enter_self(int invokingAction) {
	//Redraw border. This is *technically* not valid but it shouldn't matter
	draw_border(true);

	//Proxy event to children
	RaptorDrawable* cursor;
	while (enumerate_children(&cursor))
		cursor->hover_enter(invokingAction);

	return true;
}

void RaptorPaddedButtonDrawable::hover_leave_self() {
	//Redraw border. This is *technically* not valid but it shouldn't matter
	draw_border(false);

	//Proxy event to children
	RaptorDrawable* cursor;
	while (enumerate_children(&cursor))
		cursor->hover_leave();
}

void RaptorPaddedButtonDrawable::draw_border(bool hovering) {
	//Get border color
	raptor_pixel border = hovering ? highlighted_color : border_color;

	//Draw borders
	helper_draw_border_top(borderWidth, border);
	helper_draw_border_bottom(borderWidth, border);
	helper_draw_border_left(borderWidth, border);
	helper_draw_border_right(borderWidth, border);
}

void RaptorPaddedButtonDrawable::set_color_border(raptor_pixel color) {
	border_color = color;
	invalidate();
}

void RaptorPaddedButtonDrawable::set_color_highlighted(raptor_pixel color) {
	highlighted_color = color;
	invalidate();
}