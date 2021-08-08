#pragma once

#include "RaptorDrawable.h"
#include "RaptorFontRenderer.h"

class RaptorRdsBarDrawable : public RaptorDrawable {

	virtual int get_requested_height() override;
	virtual int get_requested_width() override;
	virtual void layout_self() override;
	virtual void draw_self() override;
	virtual void tick_self() override;
	virtual bool get_resize_allowed() override;

public:
	RaptorRdsBarDrawable();

private:
	RaptorFontRenderer renderer;
	raptor_pixel* pixelPs;
	raptor_pixel* pixelRt;
	int rtWidth;
	int rtChars;
	int pixelDividerX;

};