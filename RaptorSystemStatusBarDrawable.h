#pragma once

#include "RaptorDrawable.h"

class RaptorSystemStatusBarDrawable : public RaptorDrawable {

public:
	RaptorSystemStatusBarDrawable();

protected:
	int get_requested_height();
	bool get_resize_allowed();
	virtual void layout_self() override;
	virtual void draw_self() override;
	virtual void tick_self() override;

};