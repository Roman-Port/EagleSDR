#include "RaptorDecimatorComplex.h"

int RaptorDecimatorComplex::process(lv_32fc_t* input, lv_32fc_t* output, int count, int channels) {
	if (enabled)
		return filter->Process(input, output, count, channels);
	else
		return count;
}

void RaptorDecimatorComplex::create_filter(RaptorLowPassFilterBuilder* builder, int decimation) {
	filter = builder->BuildFilterComplex(decimation);
}