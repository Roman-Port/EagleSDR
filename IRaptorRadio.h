#pragma once
#include "RaptorSpectrumController.h"

class IRaptorRadio {

public:
	virtual void set_bandwidth(float bandwidth) = 0;

	RaptorSpectrumController spectrumMain;

};