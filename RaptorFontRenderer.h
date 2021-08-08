#pragma once

#include "RaptorPixel.h"
#include "raptor_font.h"
#include <cstddef>

class RaptorFontRenderer {

public:
	RaptorFontRenderer();
	void configure(raptor_font* font, raptor_pixel foregroundColor, raptor_pixel backgroundColor);
	void update_colors(raptor_pixel foregroundColor, raptor_pixel backgroundColor);
	
	void draw_character(raptor_pixel* ptr, char character);
	int measure_character(char character);

	void draw_line(raptor_pixel* ptr, const char* character);
	void draw_line(raptor_pixel* ptr, const char* character, int len);
	int measure_line(int len);
	int measure_line(const char* line);
	int measure_line(const char* line, int len);

	int character_spacing;
	bool monospace;

private:
	raptor_font* font;
	raptor_pixel colors[16];
	raptor_pixel* cache[256];
	int monospace_font_width;

	void unpack_character(char character);

};