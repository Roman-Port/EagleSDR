#pragma once

#include <volk/volk.h>
#include <vector>
#include <volk/volk_alloc.hh>

template <class IN_T, class OUT_T, class TAP_T>
class RaptorFirFilter {

public:
	RaptorFirFilter(const std::vector<TAP_T>& taps, int decimation);
	void set_taps(const std::vector<TAP_T>& taps);
	int process(IN_T* input, OUT_T* output, unsigned long n);

private:
	OUT_T filter(const IN_T input[]) const;
	std::vector<volk::vector<TAP_T>> d_aligned_taps;
	volk::vector<OUT_T> d_output;
	std::vector<TAP_T> d_taps;
	unsigned int d_ntaps;
	int d_align;
	int d_naligned;
	int decimation;

};

typedef RaptorFirFilter<float, float, float> RaptorFirFilter_fff;
typedef RaptorFirFilter<lv_32fc_t, lv_32fc_t, float> RaptorFirFilter_ccf;
typedef RaptorFirFilter<float, lv_32fc_t, lv_32fc_t> RaptorFirFilter_fcc;
typedef RaptorFirFilter<lv_32fc_t, lv_32fc_t, lv_32fc_t> RaptorFirFilter_ccc;