#pragma once

#include <volk/volk.h>

class RaptorRadioDemodulator {

public:
	virtual float configure(float inputSampleRate) = 0;
	virtual int demodulate(lv_32fc_t* iqBuffer, int count, float* audioBufferL, float* audioBufferR) = 0;
	virtual char* get_label() = 0;

};