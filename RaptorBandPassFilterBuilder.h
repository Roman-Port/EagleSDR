#pragma once

#include "RaptorWindowBuilder.h"
#include "RaptorFirFilterComplex.h"
#include "RaptorFilterBuilder.h"
#include "RaptorLowPassFilterBuilder.h"
#include <vector>

class RaptorBandPassFilterBuilder : public RaptorFilterBuilder {

public:
    RaptorBandPassFilterBuilder(float sampleRate, float lowCutoffFreq, float highCutoffFreq) : RaptorFilterBuilder(sampleRate) {
        this->lowCutoffFreq = lowCutoffFreq;
        this->highCutoffFreq = highCutoffFreq;
        gain = 1;
    }

    float lowCutoffFreq;
    float highCutoffFreq;
    float gain;

    float* BuildTapsReal() {
        throw std::runtime_error("Unable to create real taps for band pass filters.");
    }

    std::vector<lv_32fc_t> BuildTapsComplexVector() {
        lv_32fc_t* taps = BuildTapsComplex();
        return std::vector<lv_32fc_t>(taps, &taps[ntaps]);
    }

    lv_32fc_t* BuildTapsComplex() {
        //Construct base filter
        RaptorLowPassFilterBuilder* baseBuilder = new RaptorLowPassFilterBuilder(sampleRate, (highCutoffFreq - lowCutoffFreq) / 2);
        baseBuilder->ntaps = ntaps;
        baseBuilder->window_type = window_type;
        baseBuilder->param = param;
        float* baseTaps = baseBuilder->BuildTapsReal();
        free(baseBuilder);

        //Calculate freq
        float freq = M_PI * (highCutoffFreq + lowCutoffFreq) / sampleRate;

        //Calculate initial phase
        float phase;
        if ((ntaps & 1) != 0)
            phase = -freq * (ntaps >> 1);
        else
            phase = -freq / 2 * ((1 + 2 * ntaps) >> 1);

        //Generate
        lv_32fc_t* output = (lv_32fc_t*)malloc(sizeof(lv_32fc_t) * ntaps);
        float* outputFloat = (float*)output;
        for (int i = 0; i < ntaps; i++)
        {
            sincosf(phase, &outputFloat[0], &outputFloat[1]);
            outputFloat[0] *= baseTaps[i];
            outputFloat[1] *= baseTaps[i];
            outputFloat += 2;
            phase += freq;
        }

        //Clean up
        free(baseTaps);

        return output;
    }

protected:
    float get_max_filter_cutoff() {
        return highCutoffFreq;
    }

};