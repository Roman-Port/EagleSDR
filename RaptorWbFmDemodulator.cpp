#include "RaptorWbFmDemodulator.h";
#include "RaptorConstants.h"
#include "fast_atan2f.h"
#include "RaptorRadio.h"

#include <math.h>
#include <cstring>
#include <Iir.h>

RaptorWbFmDemodulator::RaptorWbFmDemodulator(void) {
	//Set reasonable defaults
	deviation = 75000;

	//Create the temp buffers
	demodTemp = (lv_32fc_t*)volk_malloc(RAPTOR_BUFFER_SIZE * sizeof(lv_32fc_t), volk_get_alignment());
	mpxBuffer = (float*)volk_malloc(RAPTOR_BUFFER_SIZE * sizeof(float), volk_get_alignment());
	pilotBuffer = (lv_32fc_t*)volk_malloc(RAPTOR_BUFFER_SIZE * sizeof(lv_32fc_t), volk_get_alignment());
}

float RaptorWbFmDemodulator::configure(float sampleRate) {
	//Set up gain
	gain = sampleRate / (2.5F * M_PI * deviation);
	raptor_debug_log("FM Input SampleRate: %f\n", sampleRate);

	//Set up FM filter
	fmFilter.set_label("FM MPX Decimator");
	sampleRate = fmFilter.configure(sampleRate, 64000);
	RaptorRadio::get_spectrum_mpx()->set_sample_rate((int)sampleRate);

	//Configure main pilot filter
	pilotFilterReal.setup(sampleRate, 19000, 200);

	//Configure complex pilot filter
	pilotFilterBuilder = new RaptorBandPassFilterBuilder(sampleRate, 0, sampleRate / 2);
	pilotFilterBuilder->SetAutomaticTapCount(15000, 30);
	pilotFilterBuilder->window_type = RaptorWindowBuilder::WIN_HAMMING;
	pilotFilter = pilotFilterBuilder->BuildFilterComplex(1);
	pilotFilterTest = new RaptorFirFilter< lv_32fc_t, lv_32fc_t, lv_32fc_t>(pilotFilterBuilder->BuildTapsComplexVector(), 1);
	raptor_debug_log("FM Pilot Taps %i\n", pilotFilterBuilder->ntaps);

	//Configure PLL
	pll.configure(2 * M_PI * 8 / sampleRate,
		2 * M_PI * (19000 + 4) / sampleRate,
		2 * M_PI * (19000 - 4) / sampleRate);

	//Configure and create audio filters
	float audioSampleRate = 0;
	audioFilterBuilder = new RaptorLowPassFilterBuilder(sampleRate, MAX_AUDIO_FREQ);
	audioFilterBuilder->SetAutomaticTapCount(STEREO_PILOT_FREQ - MAX_AUDIO_FREQ);
	audioFilterL = audioFilterBuilder->BuildFilterReal(audioFilterBuilder->CalculateDecimation(&audioSampleRate));
	audioFilterR = audioFilterBuilder->BuildFilterReal(audioFilterBuilder->CalculateDecimation(&audioSampleRate));
	raptor_debug_log("FM Audio SampleRate: %f\n", audioSampleRate);
	raptor_debug_log("FM Audio Taps: %i\n", audioFilterBuilder->ntaps);

	//Configure/reset deemphasis
	deemphasisAlpha = 1.0f - exp(-1.0f / (audioSampleRate * (DEEMPHASIS_ALPHA_USA * 1e-6f)));
	deemphasisStateL = 0;
	deemphasisStateR = 0;

	return audioSampleRate;
}

int RaptorWbFmDemodulator::demodulate(lv_32fc_t* iqBuffer, int count, float* audioBufferL, float* audioBufferR) {
	//Demodulate baseband FM signal
	lv_32fc_t temp;
	for (int i = 0; i < count; i++) {
		//Apply conjugate
		temp = iqBuffer[i] * conj(lastSample);

		//Estimate angle
		mpxBuffer[i] = fast_atan2f(imag(temp), real(temp)) * gain;

		//Validate that this is a real number
		if (isnan(mpxBuffer[i]))
			mpxBuffer[i] = 0;

		//Set state
		lastSample = iqBuffer[i];
	}

	//Decimate MPX
	count = fmFilter.process(mpxBuffer, mpxBuffer, count);

	//Write to spectrum
	RaptorRadio::get_spectrum_mpx()->add_samples(mpxBuffer, count);

	//Decimate and filter L + R
	int audioLength = audioFilterL->Process(mpxBuffer, audioBufferL, count, 1);

	//test
	for (int i = 0; i < count; i++) {
		if (isnan(audioBufferL[i]))
			printf("ERR\n");
	}

	//Filter stereo pilot
	for (int i = 0; i < count; i++)
		pilotBuffer[i] = pilotFilterReal.filter(mpxBuffer[i]);

	//Filter again, this time to remove the negative pilot with a complex filter
	pilotFilter->Process(pilotBuffer, pilotBuffer, count, 1);
	
	//Process PLL on pilot
	pll.process(pilotBuffer, demodTemp, count);

	//Demodulate L-R using the stereo pilot
	for (int i = 0; i < count; i++) {
		mpxBuffer[i] *= imag(demodTemp[i] * demodTemp[i]);
	}

	//If stereo was detected, apply it
	if (is_stereo_detected()) {
		//Filter and decimate L-R
		audioFilterR->Process(mpxBuffer, audioBufferR, count, 1);

		//Recover L and R audio channels
		float add;
		float sub;
		for (int i = 0; i < audioLength; i++)
		{
			add = audioBufferL[i]; //L+R signal
			sub = 2 * audioBufferR[i]; //L-R signal
			audioBufferL[i] = (add + sub);
			audioBufferR[i] = (add - sub);
		}
	}
	else {
		//Just copy from the left channel to the right channel
		printf("no stereo: %f\n", pll.get_avg_error());
		memcpy(audioBufferR, audioBufferL, audioLength * sizeof(float));
	}

	//Process deemphasis
	process_deemphasis(&deemphasisStateL, audioBufferL, audioLength);
	process_deemphasis(&deemphasisStateR, audioBufferR, audioLength);

	return audioLength;
}

bool RaptorWbFmDemodulator::is_stereo_detected() {
	return pll.get_avg_error() < 0.01f;
}

void RaptorWbFmDemodulator::process_deemphasis(float* state, float* audio, int count) {
	//Process all
	for (int i = 0; i < count; i++)
	{
		*state += deemphasisAlpha * (audio[i] - *state);
		audio[i] = *state;
	}

	//If we somehow got NaN, recover by setting our state to zero. We'll lose this block, but at least we'll recover after
	if (isnan(*state)) {
		printf("WARN: Deemphasis state was corrupted by a NaN value. Some input value was NaN. Recovering...\n");
		*state = 0;
	}
}

char* RaptorWbFmDemodulator::get_label() {
	return "WBFM";
}