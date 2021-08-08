#pragma once
#include "RaptorPaddedButtonDrawable.h"
#include "raptor_font.h"
#include "RaptorFontRenderer.h"

class RaptorTextButtonDrawable : public RaptorPaddedButtonDrawable {

	virtual void button_pressed() override;
	virtual void draw_self() override;
	virtual void layout_self() override;
	virtual void set_background_color(raptor_pixel color, float alpha = 1) override;

public:
	RaptorTextButtonDrawable(raptor_font* font, const char* text = "", raptor_pixel textColor = raptor_pixel(255, 255, 255));
	void set_text(char* text);
	void set_font_color(raptor_pixel color);

private:
	raptor_font* font;
	const char* text;
	raptor_pixel* location;
	raptor_pixel textColor;
	RaptorFontRenderer renderer;

	void update_text_location();

};