#pragma once

#include "RaptorDecimatorBase.h"
#include "RaptorFirFilterComplex.h"

class RaptorDecimatorReal : public RaptorDecimatorBase {

public:
	int process(float* input, float* output, int count, int channels = 1);

protected:
	void create_filter(RaptorLowPassFilterBuilder* builder, int decimation);

private:
	RaptorFirFilterReal *filter;

};