#include "RaptorWaterfallDrawable.h"
#include "RaptorConstants.h"
#include "RaptorWaterfallColorTable.h"
#include <cstring>

RaptorWaterfallDrawable::RaptorWaterfallDrawable(RaptorSpectrumController* spectrum) {
	this->spectrum = spectrum;
}

int RaptorWaterfallDrawable::get_requested_height()
{
	return 500;
}

int RaptorWaterfallDrawable::get_requested_width()
{
	return RAPTOR_SPECTRUM_WIDTH;
}

void RaptorWaterfallDrawable::layout_self()
{
	imageBufferSize = (get_height() - 1) * get_width(); //this will throw errors if our height is 0, FIX
	imageBuffer = (raptor_pixel*)malloc(imageBufferSize * sizeof(raptor_pixel));
	imageBufferPos = 0;
}

void RaptorWaterfallDrawable::tick_self()
{
	invalidate();
}

void RaptorWaterfallDrawable::draw_self()
{
	//First, copy the image buffer to the current display
	for (int i = get_height() - 1, src = imageBufferPos; i > 0; i--) {
		memcpy(get_pixel(0, i), &imageBuffer[src % imageBufferSize], get_width() * sizeof(raptor_pixel));
		src += get_width();
	}

	//Now, capture a frame from the spectrum
	spectrum->read(buffer);

	//Convert to row using the color table
	raptor_pixel* row = get_pixel(0, 0);
	int rowLen = std::min(RAPTOR_SPECTRUM_WIDTH, get_width());
	for (int i = 0; i < rowLen; i++)
		row[i] = RAPTOR_WATERFALL_COLORTABLE[(int)(buffer[i] * RAPTOR_WATERFALL_COLORTABLE_SCALE)];

	//Copy to the image buffer and wrap
	memcpy(&imageBuffer[imageBufferPos], row, get_width() * sizeof(raptor_pixel));
	imageBufferPos = (imageBufferPos + get_width()) % imageBufferSize;
}