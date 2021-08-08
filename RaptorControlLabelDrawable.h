#pragma once
#include "RaptorDrawable.h"
#include "RaptorFontRenderer.h"

class RaptorControlLabelDrawable : public RaptorDrawable {

	virtual void layout_self() override;
	virtual void draw_self() override;
	virtual void tick_self() override;
	virtual int get_requested_height() override;
	virtual int get_requested_width() override;

public:
	RaptorControlLabelDrawable(char* label, raptor_pixel background, RaptorDrawable* defaultChild = 0, int labelOffsetX = 0, int labelOffsetY = 0);

private:
	RaptorFontRenderer renderer;
	char* label;
	int labelOffsetX;
	int labelOffsetY;

};