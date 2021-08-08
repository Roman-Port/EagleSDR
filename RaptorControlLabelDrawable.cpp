#include "RaptorControlLabelDrawable.h"
#include "RaptorFontStore.h"
#include <algorithm>

RaptorControlLabelDrawable::RaptorControlLabelDrawable(char* label, raptor_pixel background, RaptorDrawable* defaultChild, int labelOffsetX, int labelOffsetY) {
	//Set
	this->label = label;
	this->labelOffsetX = labelOffsetX;
	this->labelOffsetY = labelOffsetY;
	set_background_color(background);

	//Configure renderer
	renderer.configure(&SYSTEM_REGULAR_15, raptor_pixel(200, 200, 200), background);

	//Add child if supplied
	if (defaultChild != 0)
		add_child(defaultChild);
}

void RaptorControlLabelDrawable::layout_self()
{
	RaptorDrawable* cursor = 0;
	while (enumerate_children(&cursor))
		cursor->request_layout(0, SYSTEM_REGULAR_15.height + labelOffsetY, get_width(), get_height() - labelOffsetY - SYSTEM_REGULAR_15.height);
}

void RaptorControlLabelDrawable::tick_self()
{
}

int RaptorControlLabelDrawable::get_requested_height() {
	return SYSTEM_REGULAR_15.height + labelOffsetY + RaptorDrawable::get_requested_height();
}

int RaptorControlLabelDrawable::get_requested_width() {
	return std::max(RaptorDrawable::get_requested_width(), renderer.measure_line(label));
}

void RaptorControlLabelDrawable::draw_self()
{
	renderer.draw_line(get_pixel(labelOffsetX, 0), label);
}