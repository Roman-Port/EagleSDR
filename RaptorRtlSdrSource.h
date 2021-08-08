#pragma once

#include <rtl-sdr.h>
#include <volk/volk.h>
#include <thread>
#include "RaptorRadioSource.h"
#include "RaptorCircularBuffer.h"

class RaptorRtlSdrSource : public RaptorRadioSource
{
public:
	bool open();
	int read(lv_32fc_t* buffer);
	void close();
	void set_center_freq(unsigned int freq);
	void set_gain_manual(int gain);
	void set_gain_auto();
	void set_sample_rate(unsigned int sampleRate);
private:
	rtlsdr_dev_t* device;
	float* conversionTable;
	unsigned char* rawBuffer;
	RaptorCircularBuffer<unsigned char>* circBuffer;
	std::thread* worker;
	float dcAvgI;
	float dcAvgQ;
	static void samples_available(unsigned char* buf, unsigned int len, void* ctx);

	inline int convert_samples(unsigned char* inputBuffer, lv_32fc_t* outputBuffer, int count) {
		//Convert samples
		float* output = (float*)outputBuffer;
		for (int i = 0; i < count; i++)
			output[i] = conversionTable[inputBuffer[i]];

		//Apply DC removal on both I and Q
		for (int i = 0; i < count; i++)
		{
			dcAvgI += 0.00001f * (output[i] - dcAvgI);
			output[i] -= dcAvgI;
			i++;
			dcAvgQ += 0.00001f * (output[i] - dcAvgQ);
			output[i] -= dcAvgQ;
		}

		return count / 2;
	}

	inline void ensure_device_ready() {
		if (device == 0)
			throw std::runtime_error("Attempted to read from RTL-SDR source while the device was null.");
	}

	inline void ensure_success(int status, const char* text) {
		if (status != 0) {
			printf("Got bad code %i in RTL-SDR source: %s\n", status, text);
			throw std::runtime_error("RTL-SDR source error.");
		}
	}
};

