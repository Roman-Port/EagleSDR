#include "RaptorDecimatorReal.h"

int RaptorDecimatorReal::process(float* input, float* output, int count, int channels) {
	if (enabled)
		return filter->Process(input, output, count, channels);
	else
		return count;
}

void RaptorDecimatorReal::create_filter(RaptorLowPassFilterBuilder* builder, int decimation) {
	filter = builder->BuildFilterReal(decimation);
}