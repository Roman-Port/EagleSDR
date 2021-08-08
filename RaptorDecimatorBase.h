#pragma once

#include "RaptorLowPassFilterBuilder.h"

class RaptorDecimatorBase {

public:
	RaptorDecimatorBase();
	void set_label(char* label);
	float configure(float sampleRate, float bandwidth);

protected:
	virtual void create_filter(RaptorLowPassFilterBuilder* builder, int decimation) = 0;
	bool enabled;

private:
	char* label;

};