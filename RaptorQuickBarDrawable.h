#pragma once

#include "RaptorMenuHorizDrawable.h"

class RaptorQuickBarDrawable : public RaptorMenuHorizDrawable {

public:
	RaptorQuickBarDrawable();

protected:
	bool get_resize_allowed();

};