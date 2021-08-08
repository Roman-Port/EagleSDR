#include "RaptorTunerDrawable.h"
#include "RaptorFontStore.h"
#include "math.h"

#define RAPTOR_TUNER_SPACING 1
#define RAPTOR_TUNER_PADDING 4

RaptorTunerDrawable::RaptorTunerDrawable(int digits, unsigned int value, raptor_tuner_callback callback) {
	this->value = value;
	this->digits = digits;
	this->callback = callback;
	decimals = ((digits + 2) / 3) - 1;
	selectedIndex = 0;

	//Configure background
	set_background_color(RAPTOR_COLOR_FORE);

	//Configure first renderer
	renderer.configure(&SYSTEM_REGULAR_20, raptor_pixel(255, 255, 255), RAPTOR_COLOR_FORE);
	renderer.monospace = true;
	renderer.character_spacing = 0;

	//Configure dark renderer
	rendererDark.configure(&SYSTEM_REGULAR_20, raptor_pixel(120, 120, 120), RAPTOR_COLOR_FORE);
	rendererDark.monospace = true;
	rendererDark.character_spacing = 0;

	//Calculate sizes
	digitWidth = RAPTOR_TUNER_SPACING + renderer.measure_character(1) + RAPTOR_TUNER_SPACING;
	digitHeight = RAPTOR_TUNER_SPACING + SYSTEM_REGULAR_20.height + RAPTOR_TUNER_SPACING;
}

void RaptorTunerDrawable::layout_self()
{
}

void RaptorTunerDrawable::tick_self()
{
}

void RaptorTunerDrawable::draw_self()
{
	//Get base pixel
	raptor_pixel* pixel = get_pixel(RAPTOR_TUNER_PADDING, RAPTOR_TUNER_PADDING);

	//Loop through groups
	int indexInverse = digits - 1;
	for (int i = 0; i < digits; i++) {
		//Draw decimal
		if (indexInverse != 0 && i != 0 && (indexInverse + 1) % 3 == 0) {
			get_renderer(indexInverse)->draw_character(pixel, '.');
			pixel += renderer.measure_character(1);
		}

		//Paint border
		if (selectedIndex == i && get_is_hovering()) {
			helper_draw_outline(pixel, RAPTOR_COLOR_SELECT, digitWidth + RAPTOR_TUNER_SPACING, digitHeight + RAPTOR_TUNER_SPACING, RAPTOR_TUNER_SPACING);
		}

		//Draw digit
		get_renderer(indexInverse)->draw_character(pixel + RAPTOR_TUNER_SPACING + (RAPTOR_TUNER_SPACING * RAPTOR_SCREEN_WIDTH), get_digit(indexInverse));
		indexInverse--;

		//Offset
		pixel += digitWidth;
	}
}

char RaptorTunerDrawable::get_digit(int index) {
	return 48 + ((value / (int)pow(10, index)) % 10);
	//     ^
	//     Ascii index of "0"
}

int RaptorTunerDrawable::get_requested_width() {
	return (digitWidth * digits) + (decimals * renderer.measure_character(1)) + (RAPTOR_TUNER_PADDING * 2);
}

int RaptorTunerDrawable::get_requested_height() {
	return digitHeight + RAPTOR_TUNER_PADDING + RAPTOR_TUNER_PADDING;
}

bool RaptorTunerDrawable::get_resize_allowed() {
	return false;
}

RaptorFontRenderer* RaptorTunerDrawable::get_renderer(int index) {
	if (pow(10, index) > value)
		return &rendererDark;
	else
		return &renderer;
}

bool RaptorTunerDrawable::process_input(int action) {
	//Decide how to handle from the input
	if (action == RAPTOR_INPUT_RIGHT && selectedIndex != digits - 1) {
		//Move cursor to the right and index is not the last item
		selectedIndex++;
		invalidate();
		return true;
	}
	if (action == RAPTOR_INPUT_LEFT && selectedIndex != 0) {
		//Move cursor to the left and index is not the first item
		selectedIndex--;
		invalidate();
		return true;
	}
	if (action == RAPTOR_INPUT_UP) {
		//Increase digit
		update_digit(selectedIndex, 1);
		return true;
	}
	if (action == RAPTOR_INPUT_DOWN) {
		//Decrease digit
		update_digit(selectedIndex, -1);
		return true;
	}
	return false;
}

bool RaptorTunerDrawable::hover_enter_self(int initiatorAction) {
	invalidate();
	selectedIndex = initiatorAction == RAPTOR_INPUT_LEFT ? digits - 1 : 0;
	return true;
}

void RaptorTunerDrawable::hover_leave_self() {
	invalidate();
}

void RaptorTunerDrawable::update_digit(int digit, int direction) {
	//Update value
	value += pow(10, digits - 1 - digit) * direction;

	//Constrain
	if (value >= pow(10, digits))
		value = pow(10, digits) - 1;
	if (value >= pow(10, digits))
		value = pow(10, digits) - 1;
	if (value < 0)
		value = 0;

	//Run callback
	callback(value);

	//Update state
	invalidate();
}