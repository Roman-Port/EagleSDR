#pragma once

#pragma once

#include "RaptorStackedDrawable.h"

class RaptorStackedVertDrawable : public RaptorStackedDrawable {

	// Inherited via RaptorStackedDrawable
	virtual int get_requested_height() override;
	virtual int get_requested_width() override;
	virtual int stacked_container_size() override;
	virtual void stacked_query(RaptorDrawable* drawable, int* requestedSize, bool* resizeAllowed) override;
	virtual void stacked_apply(RaptorDrawable* drawable, int size, int offset) override;

};