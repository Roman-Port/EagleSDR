#include "iir_filter.h"

template <>
lv_32fc_t
iir_filter<lv_32fc_t, lv_32fc_t, float, lv_32fc_t>::filter(const lv_32fc_t input)
{
    lv_32fc_t acc;
    unsigned i = 0;
    unsigned n = ntaps_ff();
    unsigned m = ntaps_fb();

    if (n == 0)
        return (lv_32fc_t)0;

    int latest_n = d_latest_n;
    int latest_m = d_latest_m;

    acc = d_fftaps[0] * input;
    for (i = 1; i < n; i++)
        acc += (d_fftaps[i] * d_prev_input[latest_n + i]);
    for (i = 1; i < m; i++)
        acc += (d_fbtaps[i] * d_prev_output[latest_m + i]);

    // store the values twice to avoid having to handle wrap-around in the loop
    d_prev_output[latest_m] = acc;
    d_prev_output[latest_m + m] = acc;
    d_prev_input[latest_n] = input;
    d_prev_input[latest_n + n] = input;

    latest_n--;
    latest_m--;
    if (latest_n < 0)
        latest_n += n;
    if (latest_m < 0)
        latest_m += m;

    d_latest_m = latest_m;
    d_latest_n = latest_n;
    return static_cast<lv_32fc_t>(acc);
}

template <>
lv_32fc_t
iir_filter<lv_32fc_t, lv_32fc_t, double, lv_64fc_t>::filter(const lv_32fc_t input)
{
    lv_64fc_t acc;
    unsigned i = 0;
    unsigned n = ntaps_ff();
    unsigned m = ntaps_fb();

    if (n == 0)
        return (lv_32fc_t)0;

    int latest_n = d_latest_n;
    int latest_m = d_latest_m;

    acc = d_fftaps[0] * static_cast<lv_64fc_t>(input);
    for (i = 1; i < n; i++)
        acc += (d_fftaps[i] * static_cast<lv_64fc_t>(d_prev_input[latest_n + i]));
    for (i = 1; i < m; i++)
        acc += (d_fbtaps[i] * static_cast<lv_64fc_t>(d_prev_output[latest_m + i]));

    // store the values twice to avoid having to handle wrap-around in the loop
    d_prev_output[latest_m] = acc;
    d_prev_output[latest_m + m] = acc;
    d_prev_input[latest_n] = input;
    d_prev_input[latest_n + n] = input;

    latest_n--;
    latest_m--;
    if (latest_n < 0)
        latest_n += n;
    if (latest_m < 0)
        latest_m += m;

    d_latest_m = latest_m;
    d_latest_n = latest_n;
    return static_cast<lv_32fc_t>(acc);
}

template <>
lv_32fc_t iir_filter<lv_32fc_t, lv_32fc_t, lv_64fc_t, lv_64fc_t>::filter(
    const lv_32fc_t input)
{
    lv_64fc_t acc;
    unsigned i = 0;
    unsigned n = ntaps_ff();
    unsigned m = ntaps_fb();

    if (n == 0)
        return (lv_32fc_t)0;

    int latest_n = d_latest_n;
    int latest_m = d_latest_m;

    acc = d_fftaps[0] * static_cast<lv_64fc_t>(input);
    for (i = 1; i < n; i++)
        acc += (d_fftaps[i] * static_cast<lv_64fc_t>(d_prev_input[latest_n + i]));
    for (i = 1; i < m; i++)
        acc += (d_fbtaps[i] * static_cast<lv_64fc_t>(d_prev_output[latest_m + i]));

    // store the values twice to avoid having to handle wrap-around in the loop
    d_prev_output[latest_m] = acc;
    d_prev_output[latest_m + m] = acc;
    d_prev_input[latest_n] = input;
    d_prev_input[latest_n + n] = input;

    latest_n--;
    latest_m--;
    if (latest_n < 0)
        latest_n += n;
    if (latest_m < 0)
        latest_m += m;

    d_latest_m = latest_m;
    d_latest_n = latest_n;
    return static_cast<lv_32fc_t>(acc);
}