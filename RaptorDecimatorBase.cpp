#include "RaptorDecimatorBase.h"
#include "RaptorConstants.h"
#include <assert.h>

RaptorDecimatorBase::RaptorDecimatorBase() {
	label = "Unnamed Decimator";
}

void RaptorDecimatorBase::set_label(char* label) {
	this->label = label;
}

float RaptorDecimatorBase::configure(float sampleRate, float bandwidth) {
	//Validate
	assert(sampleRate > 0);
	assert(bandwidth > 0);
	assert(bandwidth <= sampleRate);

	//Begin building the filter
	RaptorLowPassFilterBuilder* filterBuilder = new RaptorLowPassFilterBuilder(sampleRate, bandwidth);

	//Set a default filter cutoff. This'll be adjusted later to optimize it, but for now this is the minimum
	filterBuilder->SetAutomaticTapCount(bandwidth * 0.1F);

	//Calculate the amount we can decimate
	float outputSampleRate;
	int decimation = filterBuilder->CalculateDecimation(&outputSampleRate);

	//Debug log
	raptor_debug_log("INFO: %s: Configured from %f => %f\n", label, sampleRate, outputSampleRate);
	raptor_debug_log("INFO: %s: Configured BW: %f\n", label, bandwidth);
	raptor_debug_log("INFO: %s: Configured decimation: %i\n", label, decimation);

	//Check if we even need to be enabled
	if (decimation == 1) {
		//No need...
		enabled = false;

		//Debug log some info
		raptor_debug_log("INFO: %s: Disabling filter; decimation was 1.\n", label);
	}
	else {
		//Enable
		enabled = true;

		//Now set the cutoff to be the largest we can
		float transitionWidth = (outputSampleRate - bandwidth) / 2;
		filterBuilder->SetAutomaticTapCount(transitionWidth);

		//Validate
		assert(transitionWidth > 0);
		assert(filterBuilder->ntaps > 0);

		//Debug log some info
		raptor_debug_log("INFO: %s: Configured transition width: %f\n", label, transitionWidth);
		raptor_debug_log("INFO: %s: Configured taps: %i\n", label, filterBuilder->ntaps);

		//Build the actual filter
		create_filter(filterBuilder, decimation);
	}

	//Clean up
	free(filterBuilder);

	return outputSampleRate;
}

