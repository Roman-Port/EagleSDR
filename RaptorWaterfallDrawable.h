#pragma once

#include "RaptorDrawable.h"
#include "RaptorSpectrumController.h"

#define RAPTOR_WATERFALL_COLORTABLE_SCALE 254

class RaptorWaterfallDrawable : public RaptorDrawable {

	// Inherited via RaptorDrawable
	virtual int get_requested_height() override;
	virtual int get_requested_width() override;
	virtual void layout_self() override;
	virtual void draw_self() override;
	virtual void tick_self() override;

public:
	RaptorWaterfallDrawable(RaptorSpectrumController* spectrum);

private:
	RaptorSpectrumController* spectrum;
	float buffer[RAPTOR_SPECTRUM_WIDTH];
	raptor_pixel* imageBuffer;
	int imageBufferPos;
	int imageBufferSize;

};