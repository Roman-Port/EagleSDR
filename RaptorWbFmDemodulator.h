#pragma once

#include "RaptorRadioDemodulator.h"
#include "RaptorFirFilterReal.h"
#include "RaptorLowPassFilterBuilder.h"
#include "RaptorBandPassFilterBuilder.h"
#include "RaptorPll.h"
#include "RaptorWavFileWriter.h"
#include "RaptorFirFilter.h"
#include "RaptorDecimatorComplex.h"
#include "RaptorDecimatorReal.h"
#include "iir_filter.h"
#include <Iir.h>

const float DEEMPHASIS_ALPHA_USA = 75;

class RaptorWbFmDemodulator : public RaptorRadioDemodulator
{
	const int MIN_AUDIO_FREQ = 20;
	const int MAX_AUDIO_FREQ = 16000;
	const int STEREO_PILOT_FREQ = 19000;
	const int STEREO_PILOT_MAX_ERROR = 5;

public:
	RaptorWbFmDemodulator();
	float configure(float inputSampleRate);
	int demodulate(lv_32fc_t* iqBuffer, int count, float* audioBufferL, float* audioBufferR);
	char* get_label();
	bool is_stereo_detected();
private:
	lv_32fc_t* demodTemp;
	lv_32fc_t* pilotBuffer;
	float* mpxBuffer;
	float gain;
	float deviation;
	RaptorPll pll;
	lv_32fc_t lastSample;

	//filters right off of the conjugate calculation
	RaptorDecimatorReal fmFilter;
	float fmFilterSampleRate;

	//Stereo pilot filter
	Iir::Butterworth::BandPass<4> pilotFilterReal;
	RaptorFirFilterComplex* pilotFilter;
	RaptorBandPassFilterBuilder* pilotFilterBuilder;
	RaptorFirFilter<lv_32fc_t, lv_32fc_t, lv_32fc_t>* pilotFilterTest;

	//Audio filters
	RaptorFirFilterReal* audioFilterL;
	RaptorFirFilterReal* audioFilterR;
	RaptorLowPassFilterBuilder* audioFilterBuilder;

	//Deemphasis
	float deemphasisAlpha;
	float deemphasisStateL;
	float deemphasisStateR;

	void process_deemphasis(float* state, float* audio, int count);

};
