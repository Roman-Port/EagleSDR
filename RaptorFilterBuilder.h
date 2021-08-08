#pragma once

#include <volk/volk.h>
#include "RaptorWindowBuilder.h"
#include "RaptorFirFilterComplex.h"
#include "RaptorFirFilterReal.h"

class RaptorFilterBuilder {

public:
	RaptorFilterBuilder(float sampleRate) {
		this->sampleRate = sampleRate;
		ntaps = 0;
		window_type = RaptorWindowBuilder::WIN_BLACKMAN_HARRIS;
		param = 0;
	}

	float sampleRate;
	int ntaps;
	RaptorWindowBuilder::win_type window_type;
	double param;

	void SetAutomaticTapCount(float transitionWidth, float attenuation = 30) {
		this->transitionWidth = transitionWidth;
		int count = (int)(attenuation / (22 * (transitionWidth / sampleRate)));
		if ((count & 1) == 0) //If this is odd, make it even
			count++;
		ntaps = count;
	}

	RaptorFirFilterComplex* BuildFilterComplex(int decimation) {
		return new RaptorFirFilterComplex(BuildTapsComplex(), ntaps, decimation);
	}

	RaptorFirFilterReal* BuildFilterReal(int decimation) {
		return new RaptorFirFilterReal(BuildTapsReal(), ntaps, decimation);
	}

	int CalculateDecimation(float* actualOutputSampleRate) {
		//Calculate signal bandwidth
		float bandwidth = get_max_filter_cutoff() + transitionWidth;

		//Calculate the rate by finding the LOWEST we can go without it becoming a rate lower than the desired rate
		int decimationRate = 1;
		while (sampleRate / (decimationRate + 1) >= (bandwidth * 2)) //Multiply the bandwidth so we can run this without any aliasing
		{
			decimationRate++;
		}

		//Determine the actual output sample rate
		*actualOutputSampleRate = sampleRate / decimationRate;

		return decimationRate;
	}

	virtual float* BuildTapsReal() = 0;

	virtual lv_32fc_t* BuildTapsComplex() {
		//Build real taps
		float* real = BuildTapsReal();

		//Allocate complex taps instead
		float* converted = (float*)malloc(sizeof(lv_32fc_t) * ntaps);

		//Convert
		for (int i = 0; i < ntaps; i++) {
			converted[(i * 2) + 0] = real[i];
			converted[(i * 2) + 1] = real[i];
		}

		//Clean up
		free(real);

		return (lv_32fc_t*)converted;
	}

protected:
	float transitionWidth;
	virtual float get_max_filter_cutoff() = 0;

};