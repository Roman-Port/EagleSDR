#include "RaptorFirFilter.h"
#include <cstdint>
#include <algorithm>

template <class IN_T, class OUT_T, class TAP_T>
RaptorFirFilter<IN_T, OUT_T, TAP_T>::RaptorFirFilter(const std::vector<TAP_T>& taps, int decimation) : d_output(1)
{
    d_align = volk_get_alignment();
    d_naligned = std::max((size_t)1, d_align / sizeof(IN_T));
    set_taps(taps);
    this->decimation = decimation;
}

template <class IN_T, class OUT_T, class TAP_T>
void RaptorFirFilter<IN_T, OUT_T, TAP_T>::set_taps(const std::vector<TAP_T>& taps)
{
    d_ntaps = (int)taps.size();
    d_taps = taps;
    std::reverse(d_taps.begin(), d_taps.end());

    d_aligned_taps.clear();
    d_aligned_taps = std::vector<volk::vector<TAP_T>>(
        d_naligned, volk::vector<TAP_T>((d_ntaps + d_naligned - 1), 0));
    for (int i = 0; i < d_naligned; i++) {
        for (unsigned int j = 0; j < d_ntaps; j++)
            d_aligned_taps[i][i + j] = d_taps[j];
    }
}

template <class IN_T, class OUT_T, class TAP_T>
int RaptorFirFilter<IN_T, OUT_T, TAP_T>::process(IN_T* input, OUT_T* output, unsigned long n)
{
    unsigned long j = 0;
    int processed = 0;
    for (unsigned long i = 0; i < n; i++) {
        output[i] = filter(&input[j]);
        j += decimation;
        processed++;
    }
    return processed;
}

/* Various implementations... */

template <>
float RaptorFirFilter<float, float, float>::filter(const float input[]) const
{
    const float* ar = (float*)((size_t)input & ~(d_align - 1));
    unsigned al = input - ar;

    volk_32f_x2_dot_prod_32f_a(
        const_cast<float*>(d_output.data()), ar, d_aligned_taps[al].data(), d_ntaps + al);
    return d_output[0];
}

template <>
lv_32fc_t
RaptorFirFilter<lv_32fc_t, lv_32fc_t, float>::filter(const lv_32fc_t input[]) const
{
    const lv_32fc_t* ar = (lv_32fc_t*)((size_t)input & ~(d_align - 1));
    unsigned al = input - ar;

    volk_32fc_32f_dot_prod_32fc_a(const_cast<lv_32fc_t*>(d_output.data()),
        ar,
        d_aligned_taps[al].data(),
        (d_ntaps + al));
    return d_output[0];
}

template <>
lv_32fc_t RaptorFirFilter<float, lv_32fc_t, lv_32fc_t>::filter(const float input[]) const
{
    const float* ar = (float*)((size_t)input & ~(d_align - 1));
    unsigned al = input - ar;

    volk_32fc_32f_dot_prod_32fc_a(const_cast<lv_32fc_t*>(d_output.data()),
        d_aligned_taps[al].data(),
        ar,
        (d_ntaps + al));
    return d_output[0];
}

template <>
lv_32fc_t
RaptorFirFilter<lv_32fc_t, lv_32fc_t, lv_32fc_t>::filter(const lv_32fc_t input[]) const
{
    const lv_32fc_t* ar = (lv_32fc_t*)((size_t)input & ~(d_align - 1));
    unsigned al = input - ar;

    volk_32fc_x2_dot_prod_32fc_a(const_cast<lv_32fc_t*>(d_output.data()),
        ar,
        d_aligned_taps[al].data(),
        (d_ntaps + al));
    return d_output[0];
}

template class RaptorFirFilter<float, float, float>;
template class RaptorFirFilter<lv_32fc_t, lv_32fc_t, float>;
template class RaptorFirFilter<float, lv_32fc_t, lv_32fc_t>;
template class RaptorFirFilter<lv_32fc_t, lv_32fc_t, lv_32fc_t>;