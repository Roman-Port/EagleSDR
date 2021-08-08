#pragma once

#include "RaptorDrawable.h"

class RaptorStackedDrawable : public RaptorDrawable {

	void layout_self();
	void draw_self();
	void tick_self();

public:
	RaptorStackedDrawable();
	void set_padding(int padding);
	void set_padding(int paddingHoriz, int paddingVert);
	void set_padding(int paddingLeft, int paddingTop, int paddingRight, int paddingBottom);
	void set_spacing(int spacing);

protected:
	virtual int stacked_container_size() = 0;
	virtual void stacked_query(RaptorDrawable* drawable, int* requestedSize, bool* resizeAllowed) = 0;
	virtual void stacked_apply(RaptorDrawable* drawable, int size, int offset) = 0;
	int padding_top;
	int padding_bottom;
	int padding_left;
	int padding_right;
	int spacing;

};