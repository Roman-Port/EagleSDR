#pragma once
#include "RaptorSystemBarIcon.h"

class RaptorSysIconSignal : public RaptorSystemBarIcon {

	virtual int update(raptor_pixel* ptr, int height) override;
};