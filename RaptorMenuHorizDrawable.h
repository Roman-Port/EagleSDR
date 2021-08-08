#pragma once

#include "RaptorMenuHelper.h"
#include "RaptorStackedHorizDrawable.h"

class RaptorMenuHorizDrawable : public RaptorStackedHorizDrawable {

	bool process_input(int action);
	bool hover_enter_self();
	void hover_leave_self();

public:
	RaptorMenuHorizDrawable();

private:
	RaptorMenuHelper helper;

};