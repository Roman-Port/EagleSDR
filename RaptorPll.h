#pragma once

#include <cstdio>
#include <math.h>
#include <volk/volk.h>
#include "fast_atan2f.h"

//From GNURadio, GPL-3.0-or-later

class RaptorPll {

public:
	RaptorPll() {
        
	}

    void configure(float loop_bw, float max_freq, float min_freq);
    void process(lv_32fc_t* iptr, lv_32fc_t* optr, int count);

    inline float get_avg_error() {
        return d_avg_error;
    }

private:

    float phase_detector(lv_32fc_t sample, float ref_phase);
    float mod_2pi(float in);
    void frequency_limit();
    void phase_wrap();
    void advance_loop(float error);

    float d_phase, d_freq;
    float d_max_freq, d_min_freq;
    float d_damping, d_loop_bw;
    float d_alpha, d_beta;

    float d_alpha_inverse, d_avg_error;

};