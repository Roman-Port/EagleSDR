#include "RaptorFontRenderer.h"
#include <stdlib.h>
#include <cstring>
#include "RaptorConstants.h"
#include <algorithm>
#include <stdexcept>

#define RAPTOR_FONT_MIN_WIDTH 5

RaptorFontRenderer::RaptorFontRenderer() {
	//Clear and set defaults
	font = 0;
	character_spacing = 1;
	monospace = false;

	//Zero out the cache, as it'll be generated later
	for (int i = 0; i < 256; i++)
		cache[i] = 0;
}

void RaptorFontRenderer::configure(raptor_font* font, raptor_pixel foregroundColor, raptor_pixel backgroundColor) {
	//Set font
	this->font = font;

	//Update our colors
	update_colors(foregroundColor, backgroundColor);

	//Calculate monospace width by finding the max width
	monospace_font_width = 0;
	for (int i = 0; i < 256; i++)
		monospace_font_width = std::max(monospace_font_width, font->width[i]);
}

void RaptorFontRenderer::update_colors(raptor_pixel foregroundColor, raptor_pixel backgroundColor) {
	//Calculate color mix for each supported level
	for (int i = 0; i < 16; i++) {
		//Calculate ratio
		float ratioA = ((float)i / 15);
		float ratioB = 1 - ratioA;

		//Apply
		colors[i].r = (unsigned char)((foregroundColor.r * ratioA) + (backgroundColor.r * ratioB));
		colors[i].g = (unsigned char)((foregroundColor.g * ratioA) + (backgroundColor.g * ratioB));
		colors[i].b = (unsigned char)((foregroundColor.b * ratioA) + (backgroundColor.b * ratioB));
	}

	//Clear cache
	for (int i = 0; i < 256; i++) {
		if (cache[i] != 0) {
			free(cache[i]);
			cache[i] = 0;
		}
	}
}

void RaptorFontRenderer::draw_character(raptor_pixel* dst, char character) {
	//Get the character from cache
	raptor_pixel* src = cache[(unsigned char)character];
	
	//If it's not yet unpacked, unpack it
	if (src == 0) {
		unpack_character(character);
		src = cache[(unsigned char)character];
	}

	//If we're in monospaced mode, offset the font accordingly
	if (monospace)
		dst += (monospace_font_width - font->width[(unsigned char)character] + 1) / 2;

	//Read
	for (int y = 0; y < font->height; y++) {
		memcpy(dst, src, font->width[(unsigned char)character] * sizeof(raptor_pixel));
		src += font->width[(unsigned char)character];
		dst += RAPTOR_SCREEN_WIDTH;
	}
}

void RaptorFontRenderer::unpack_character(char character) {
	//Allocate space for the unpacked character
	raptor_pixel* unpacked = (raptor_pixel*)malloc(font->width[(unsigned char)character] * font->height * sizeof(raptor_pixel));

	//Get pointer to the packed data
	const unsigned char* packed = (const unsigned char*)font->charset[(unsigned char)character];

	//Register in cache
	cache[(unsigned char)character] = unpacked;

	//Begin unpacking in blocks
	int size = font->width[(unsigned char)character] * font->height;
	for (int i = 0; i < size; i++)
		*unpacked++ = colors[i % 2 == 0 ? packed[i / 2] >> 4 : packed[i / 2] & 15];
}

int RaptorFontRenderer::measure_character(char character) {
	if (monospace)
		return monospace_font_width;
	else
		return std::max(RAPTOR_FONT_MIN_WIDTH, font->width[(unsigned char)character]);
}

void RaptorFontRenderer::draw_line(raptor_pixel* dst, const char* text) {
	draw_line(dst, text, strlen(text));
}

void RaptorFontRenderer::draw_line(raptor_pixel* dst, const char* text, int len) {
	//Draw each
	for (int i = 0; i < len; i++) {
		draw_character(dst, text[i]);
		dst += measure_character(text[i]);
	}
}

int RaptorFontRenderer::measure_line(const char* line) {
	return measure_line(line, strlen(line));
}

int RaptorFontRenderer::measure_line(const char* line, int len) {
	//Calculate each
	int length = 0;
	for (int i = 0; i < len; i++) {
		//Add length of character
		length += measure_character(line[i]);

		//Add spacing if this isn't the last one
		if(i != len - 1)
			length += character_spacing;
	}

	return length;
}

int RaptorFontRenderer::measure_line(int len) {
	//This only works in monospaced mode, so make sure we're in that
	if (!monospace)
		throw std::runtime_error("measure_line(len) can only be used in monospaced text mode!");

	//Calculate
	return (len * monospace_font_width) + (std::max(0, (len - 1)) * character_spacing);
}