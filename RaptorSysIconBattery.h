#pragma once
#include "RaptorSystemBarIcon.h"

class RaptorSysIconBattery : public RaptorSystemBarIcon {

	virtual int update(raptor_pixel* ptr, int height) override;

};