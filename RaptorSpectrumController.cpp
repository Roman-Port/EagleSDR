#include "RaptorSpectrumController.h"
#include <cstring>
#include <algorithm>
#include <fftw3.h>
#include <math.h>
#include "RaptorWindowBuilder.h"

RaptorSpectrumController::RaptorSpectrumController() : fftBufferLockNotifierUnique(fftBufferLock) {
    frameExported = false;
    samplesToFrame = 0;
    bufferPos = 0;

    attack = 0.6f;
    decay = 0.5f;
    offset = 30;
    range = 80;
}

void RaptorSpectrumController::setup(int fftBins, int frameWidth, int fps, bool realMode) {
	//Configure
	this->fftBins = fftBins;
    this->frameWidth = frameWidth;
    this->fps = fps;
    this->realMode = realMode;

	//Create buffers
	loopBuffer = (lv_32fc_t*)malloc(sizeof(lv_32fc_t) * fftBins);
	exportBuffer = (lv_32fc_t*)fftw_malloc(sizeof(lv_32fc_t) * fftBins);
	fftInBuffer = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fftBins);
	fftOutBuffer = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fftBins);
	computedPower = (float*)fftw_malloc(sizeof(float) * fftBins);
	resizedPower = (float*)fftw_malloc(sizeof(float) * frameWidth);
	avgPower = (float*)fftw_malloc(sizeof(float) * frameWidth);

    //Zero out buffers
    memset(loopBuffer, 0, sizeof(lv_32fc_t) * fftBins);
    memset(exportBuffer, 0, sizeof(lv_32fc_t) * fftBins);
    memset(fftInBuffer, 0, sizeof(fftw_complex) * fftBins);
    memset(fftOutBuffer, 0, sizeof(fftw_complex) * fftBins);
    memset(computedPower, 0, sizeof(float) * fftBins);
    memset(resizedPower, 0, sizeof(float) * frameWidth);
    memset(avgPower, 0, sizeof(float) * frameWidth);

    //Generate FFT window and offset
    windowBuffer = RaptorWindowBuilder::blackman_harris(fftBins);
    /*float* windowBufferInverse = &windowBuffer[(fftBins - 1) / 2];
    for (int i = 0; i < fftBins / 2; i++) {
        float temp = windowBuffer[i];
        windowBuffer[i] = windowBufferInverse[i];
        windowBufferInverse[i] = temp;
    }*/

    //Configure fftw
    fftw = fftw_plan_dft_1d(fftBins, fftInBuffer, fftOutBuffer, FFTW_FORWARD, FFTW_MEASURE);

    //Calculate normalization factor
    normalizationFactor = 1.0f / fftBins;

    //Spawn worker thread
    workerThread = new std::thread(worker_static, this);
}

void RaptorSpectrumController::configure() {
    frameSize = sampleRate / fps;
    samplesToFrame = frameSize;
}

void RaptorSpectrumController::add_samples(lv_32fc_t* buffer, int count) {
    while (count > 0 && frameSize != 0)
    {
        //Determine how much is writable
        int writable = std::min(std::min(samplesToFrame, fftBins - bufferPos), count);

        //Transfer and update state
        memcpy(loopBuffer + bufferPos, buffer, writable * sizeof(lv_32fc_t));
        bufferPos = (bufferPos + writable) % fftBins;
        samplesToFrame -= writable;
        buffer += writable;
        count -= writable;

        //Check if it's time to export a frame
        if (samplesToFrame == 0)
            export_frame();
    }
}

void RaptorSpectrumController::add_samples(float* buffer, int count) {
    while (count > 0 && frameSize != 0)
    {
        //Determine how much is writable
        int writable = std::min(std::min(samplesToFrame, fftBins - bufferPos), count);

        //Transfer and update state
        for (int i = 0; i < writable; i++)
            loopBuffer[bufferPos + i] = lv_32fc_t(buffer[i], 0);
        bufferPos = (bufferPos + writable) % fftBins;
        samplesToFrame -= writable;
        buffer += writable;
        count -= writable;

        //Check if it's time to export a frame
        if (samplesToFrame == 0)
            export_frame();
    }
}

void RaptorSpectrumController::export_frame() {
    //Transfer to buffer 
    memcpy(exportBuffer, loopBuffer + bufferPos, (fftBins - bufferPos) * sizeof(lv_32fc_t));
    memcpy(exportBuffer + (fftBins - bufferPos), loopBuffer, bufferPos * sizeof(lv_32fc_t));

    //Notify worker thread of export
    frameExported = true;
    fftBufferLockNotifier.notify_one();

    //Reset
    samplesToFrame = frameSize;
}

void RaptorSpectrumController::worker() {
    while (true) {
        //Idle until we have a frame exported
        while(!frameExported)
            fftBufferLockNotifier.wait(fftBufferLockNotifierUnique);

        //Convert from float to double complex for FFTW
        for (int i = 0; i < fftBins; i++) {
            fftInBuffer[i][0] = real(exportBuffer[i]) * windowBuffer[i];
            fftInBuffer[i][1] = imag(exportBuffer[i]) * windowBuffer[i];
        }

        //Reset state
        frameExported = false;
        fftBufferLock.unlock();

        //Compute FFT
        fftw_execute(fftw);

        //Calculate power from FFT
        calculate_power(fftOutBuffer, computedPower, normalizationFactor, fftBins);

        //Resize. We also chop the spectrum in half if this is in real mode
        if (!realMode)
            resize_power(computedPower, resizedPower, fftBins, frameWidth);
        else
            resize_power(computedPower + (fftBins / 2), resizedPower, fftBins / 2, frameWidth);

        //Smoothen
        apply_smoothening(avgPower, resizedPower, frameWidth, attack, decay);
    }
}

void RaptorSpectrumController::read(float* output) {
    //Read raw first
    read_raw(output);

    //Flip sign, adjust range and constrain
    for (int i = 0; i < frameWidth; i++) {
        output[i] = -output[i];
        output[i] -= offset;
        output[i] /= range;
        output[i] = std::max(0.0f, output[i]);
        output[i] = std::min(1.0f, output[i]);
    }
}

void RaptorSpectrumController::read_raw(float* output) {
    memcpy(output, avgPower, frameWidth * sizeof(float));
}

void RaptorSpectrumController::set_sample_rate(int sampleRate) {
    this->sampleRate = sampleRate;
    configure();
}

bool RaptorSpectrumController::get_sample_rate(int* sampleRate) {
    if (*sampleRate != this->sampleRate) {
        *sampleRate = this->sampleRate;
        return true;
    }
    else {
        return false;
    }
}

void RaptorSpectrumController::calculate_power(fftw_complex* input, float* power, float normalizationFactor, int fftBins) {
    float real;
    float imag;
    for (int i = 0, o = fftBins / 2; i < fftBins; i++)
    {
        //Normalize
        real = input[i][0] * normalizationFactor;
        imag = input[i][1] * normalizationFactor;

        //Calculate power and also offset it to the middle
        power[o++ % fftBins] = (float)(10.0 * std::log10(((real * real) + (imag * imag)) + 1e-20));
    }
}

void RaptorSpectrumController::resize_power(float* input, float* output, int inputLen, int outputLen) {
    int inputIndex = 0;
    int outputIndex = 0;
    float max = 0;
    bool resetMax = true;
    while (inputIndex < inputLen)
    {
        //Read value
        if (resetMax)
            max = input[inputIndex++];
        else
            max = std::max(input[inputIndex++], max);

        //Write to output
        int targetOutput = (inputIndex * outputLen) / inputLen;
        resetMax = (outputIndex < targetOutput);
        while (outputIndex < targetOutput)
            output[outputIndex++] = max;
    }
}

void RaptorSpectrumController::apply_smoothening(float* buffer, float* incoming, int count, float attack, float decay) {
    float ratio;
    for (int i = 0; i < count; i++)
    {
        ratio = buffer[i] < incoming[i] ? attack : decay;
        buffer[i] = buffer[i] * (1 - ratio) + incoming[i] * ratio;
    }
}