#pragma once

#include <volk/volk.h>
#include <mutex>
#include <condition_variable>
#include <fftw3.h>
#include <thread>

class RaptorSpectrumController {

public:
	RaptorSpectrumController();
	void setup(int fftBins, int frameWidth, int fps, bool realMode);
	void add_samples(lv_32fc_t* ptr, int count);
	void add_samples(float* ptr, int count);
	void read(float* output);
	void read_raw(float* output);
	void set_sample_rate(int sampleRate);
	bool get_sample_rate(int* sampleRate);

	//NO TOUCH!
	int fftBins;
	int frameWidth;
	

	//Touch OK
	float attack;
	float decay;
	float range;
	float offset;

private:
	//FFT buffer
	int frameSize;
	int bufferPos;
	int samplesToFrame;
	bool frameExported;
	lv_32fc_t* loopBuffer;
	lv_32fc_t* exportBuffer;
	float* windowBuffer;
	std::mutex fftBufferLock;
	std::condition_variable fftBufferLockNotifier;
	std::unique_lock<std::mutex> fftBufferLockNotifierUnique;
	void export_frame();
	int sampleRate;
	int fps;
	void configure();

	//FFTW
	fftw_plan fftw;
	fftw_complex* fftInBuffer;
	fftw_complex* fftOutBuffer;

	//Post-processing
	bool realMode;
	float normalizationFactor;
	float* computedPower;
	float* resizedPower;
	float* avgPower;
	static void calculate_power(fftw_complex* input, float* power, float normalizationFactor, int fftLen);
	static void resize_power(float* input, float* output, int inputLen, int outputLen);
	static void apply_smoothening(float* buffer, float* incoming, int count, float attack, float decay);

	//Worker
	std::thread* workerThread;
	void worker();
	static void worker_static(RaptorSpectrumController* ctx) {
		ctx->worker();
	}

};