#pragma once

#include "RaptorRadioSource.h"
#include "RaptorConstants.h"
#include <volk/volk.h>

class RaptorFileSource : public RaptorRadioSource {

public:
	RaptorFileSource(char* filename);
	bool open();
	int read(lv_32fc_t* buffer);
	void close();
	void set_center_freq(unsigned int freq);

private:
	char* filename;
	FILE* file;
	unsigned char header[44];
	short rawBuffer[RAPTOR_BUFFER_SIZE * 2];

};