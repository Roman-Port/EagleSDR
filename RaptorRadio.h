#pragma once

#include "RaptorSpectrumController.h"

#define RAPTOR_SOURCE_COUNT 2
#define RAPTOR_DEMODULATOR_COUNT 1

namespace RaptorRadio
{
	bool initialize(char** errorMsg);
	void run();

	void set_frequency(unsigned int freq);
	void set_bandwidth(unsigned int bandwidth);
	void set_demodulator(int index);

	RaptorSpectrumController* get_spectrum_main();
	RaptorSpectrumController* get_spectrum_mpx();
	char** get_demodulator_labels();
}