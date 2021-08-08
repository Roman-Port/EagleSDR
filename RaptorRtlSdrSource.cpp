#include "RaptorRtlSdrSource.h"
#include "RaptorConstants.h"
#include <volk/volk.h>
#include <rtl-sdr.h>

const int RAPTOR_RTLSDR_CIRC_BUFFER = RAPTOR_BUFFER_SIZE * 10;

bool RaptorRtlSdrSource::open() {
	//First, try to actually open the device
	if (rtlsdr_open(&device, 0) != 0)
		return false;

	//Generate the conversion table (byte to float)
	conversionTable = (float*)malloc(sizeof(float) * 256);
	for (short i = 0; i < 256; i++)
		conversionTable[i] = ((i - 127.5f) / 127.5f);

	//Create the raw buffer for holding the BYTES used as an input
	rawBuffer = (unsigned char*)malloc(RAPTOR_BUFFER_SIZE * 2);

	//Create circular buffer
	circBuffer = new RaptorCircularBuffer<unsigned char>(sizeof(lv_32fc_t), RAPTOR_RTLSDR_CIRC_BUFFER);

	//Configure with sensible defaults
	set_gain_manual(3);
	set_center_freq(92500000);
	//set_sample_rate(2400000);
	set_sample_rate(900001);

	//Reset buffer
	ensure_success(rtlsdr_reset_buffer(device), "Clearing device buffer");

	//Clear
	dcAvgI = 0;
	dcAvgQ = 0;

	//Create worker
	worker = new std::thread(rtlsdr_read_async, device, samples_available, this, 0, RAPTOR_BUFFER_SIZE);

	return true;
}

int RaptorRtlSdrSource::read(lv_32fc_t* buffer) {
	//Read from buffer
	int read = circBuffer->read(rawBuffer, RAPTOR_BUFFER_SIZE * 2);

	//Convert samples
	read = convert_samples(rawBuffer, buffer, read);

	return read;
}

void RaptorRtlSdrSource::close() {
	
}

void RaptorRtlSdrSource::set_center_freq(unsigned int freq) {
	//If the device isn't valid and we're trying to read, abort
	ensure_device_ready();

	//Apply
	rtlsdr_set_center_freq(device, freq);
}

void RaptorRtlSdrSource::set_gain_manual(int gain) {
	//If the device isn't valid and we're trying to read, abort
	ensure_device_ready();

	//Make sure auto gain is off
	rtlsdr_set_tuner_gain_mode(device, 1);

	//Apply
	rtlsdr_set_tuner_gain(device, gain);
}

void RaptorRtlSdrSource::set_gain_auto() {
	//If the device isn't valid and we're trying to read, abort
	ensure_device_ready();

	//Apply
	rtlsdr_set_tuner_gain_mode(device, 0);
}

void RaptorRtlSdrSource::set_sample_rate(unsigned int sampleRate) {
	//If the device isn't valid and we're trying to read, abort
	ensure_device_ready();

	//Apply
	rtlsdr_set_sample_rate(device, sampleRate);
	this->sample_rate = sampleRate;
}

void RaptorRtlSdrSource::samples_available(unsigned char* buf, unsigned int count, void* ctxRaw) {
	//Convert
	RaptorRtlSdrSource* ctx = (RaptorRtlSdrSource*)ctxRaw;

	//Send to buffer
	int dropped = count - ctx->circBuffer->write(buf, count);

	//Warn if samples were dropped
	if (dropped > 0)
		printf("WARN: RTL-SDR source dropped %i samples!\n", dropped);
}