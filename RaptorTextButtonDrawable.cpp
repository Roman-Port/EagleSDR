#include "RaptorTextButtonDrawable.h"
#include <cstring>

RaptorTextButtonDrawable::RaptorTextButtonDrawable(raptor_font* font, const char* text, raptor_pixel textColor) {
	//Set
	this->font = font;
	this->textColor = textColor;
	this->text = text;

	//Configure
	renderer.configure(font, textColor, get_background_color());
}

void RaptorTextButtonDrawable::button_pressed()
{
}

void RaptorTextButtonDrawable::draw_self()
{
	renderer.draw_line(location, text);
}

void RaptorTextButtonDrawable::layout_self()
{
	update_text_location();
}

void RaptorTextButtonDrawable::set_text(char* text)
{
	this->text = text;
	update_text_location();
}

void RaptorTextButtonDrawable::set_font_color(raptor_pixel color)
{
	textColor = color;
	renderer.update_colors(color, get_background_color());
}

void RaptorTextButtonDrawable::set_background_color(raptor_pixel color, float alpha)
{
	RaptorPaddedButtonDrawable::set_background_color(color, alpha);
	renderer.update_colors(textColor, get_background_color());
}

void RaptorTextButtonDrawable::update_text_location()
{
	//Calculate the text and center it
	int y = (get_height() / 2) - (font->height / 2);
	int x = (get_width() / 2) - (renderer.measure_line(text) / 2);

	//Position
	location = get_pixel(x, y);
}