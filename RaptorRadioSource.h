#pragma once

#include <volk/volk.h>

class RaptorRadioSource {

public:

	virtual bool open() = 0;
	virtual int read(lv_32fc_t* buffer) = 0;
	virtual void close() = 0;
	virtual void set_center_freq(unsigned int freq) = 0;
	int sample_rate = 0;

};