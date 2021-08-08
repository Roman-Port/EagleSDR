#pragma once

#include "RaptorConstants.h"
#include <cstdio>
#include <cstdlib>
#include <libresample.h>
#include <stdexcept>
#include <cstring>

class RaptorArbitraryResampler {

public:
	RaptorArbitraryResampler() {
		//Set defaults
		factor = 0;
		handle = 0;

		//Create internal buffer
		internalBufferSize = RAPTOR_BUFFER_SIZE * 2;
		internalBuffer = (float*)malloc(internalBufferSize * sizeof(float));
		internalBufferUsage = 0;
		internalBufferReadPos = 0;
		internalBufferWritePos = 0;
	}

	void configure(double inputSampleRate, double outputSampleRate) {
		//Calculate factor
		factor = outputSampleRate / inputSampleRate;

		//Close existing resampler instance, if any
		if (handle != 0)
			resample_close(handle);

		//Create resampler instance
		handle = resample_open(1, factor, factor);
	}

	void input(float* buffer, int count) {
		//Make sure we have space
		if (internalBufferSize - internalBufferUsage < count)
			throw std::runtime_error("Not enough space in arbitary resampler buffer!");

		//Write to internal circular buffer
		while (count > 0) {
			int writable = std::min(count, internalBufferSize - internalBufferWritePos);
			memcpy(&internalBuffer[internalBufferWritePos], buffer, writable * sizeof(float));
			internalBufferWritePos = (internalBufferWritePos + writable) % internalBufferSize;
			internalBufferUsage += writable;
			count -= writable;
			buffer += writable;
		}
	}

	int output(float* buffer, int count) {
		//Validate
		ensure_resampler();

		//Process
		int inBufferUsed = std::min(internalBufferSize - internalBufferReadPos, internalBufferUsage);
		count = resample_process(handle, factor, &internalBuffer[internalBufferReadPos], inBufferUsed, 0, &inBufferUsed, buffer, count);

		//Update circular buffer state
		internalBufferReadPos = (internalBufferReadPos + inBufferUsed) % internalBufferSize;
		internalBufferUsage -= inBufferUsed;

		return count;
	}

private:
	void* handle;
	double factor;
	float* internalBuffer;
	int internalBufferSize;
	int internalBufferUsage;
	int internalBufferReadPos;
	int internalBufferWritePos;

	inline void ensure_resampler() {
		if (handle == 0)
			throw std::runtime_error("Attempted to use resampler before one was created.");
	}

};