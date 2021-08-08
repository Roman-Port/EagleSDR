#pragma once

#include "RaptorDecimatorBase.h"
#include "RaptorFirFilterComplex.h"

class RaptorDecimatorComplex : public RaptorDecimatorBase {

public:
	int process(lv_32fc_t* input, lv_32fc_t* output, int count, int channels = 1);

protected:
	void create_filter(RaptorLowPassFilterBuilder* builder, int decimation);

private:
	RaptorFirFilterComplex* filter;

};