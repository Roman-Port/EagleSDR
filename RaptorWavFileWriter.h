#pragma once
#include <cstdio>
#include "RaptorConstants.h"
#include <algorithm>
#include <volk/volk.h>

class RaptorWavFileWriter {

public:
	RaptorWavFileWriter(const char* path, int sampleRate) {
		//Open file
		handle = fopen(path, "wb+");

		//Write header
		unsigned char* sampleRateBytes = (unsigned char*)&sampleRate;
		char header[] = { '\x52', '\x49', '\x46', '\x46', '\xFE', '\xFF', '\xFF', '\x7F', '\x57', '\x41', '\x56', '\x45', '\x66', '\x6D', '\x74', '\x20', '\x10', '\x00', '\x00', '\x00', '\x01', '\x00', '\x02', '\x00', sampleRateBytes[0], sampleRateBytes[1], sampleRateBytes[2], sampleRateBytes[3], '\x00', '\x1B', '\xB7', '\x00', '\x04', '\x00', '\x10', '\x00', '\x64', '\x61', '\x74', '\x61', '\xFF', '\xFF', '\xFF', '\x7F' };
		fwrite(header, sizeof(char), 44, handle);
	}

	void write(float* left, float* right, int count) {
		while (count > 0) {
			//Determine number of writable objects
			int writable = std::min(count, RAPTOR_BUFFER_SIZE);

			//Transform to buffer
			short* endpoint = tempBuffer;
			for (int i = 0; i < writable; i++) {
				transfer(endpoint++, left++);
				transfer(endpoint++, right++);
			}

			//Write to file
			fwrite(tempBuffer, sizeof(short), writable * 2, handle);

			//Update state
			count -= writable;
		}
	}

	void write(lv_32fc_t* samples, int count) {
		//Update
		float* data = (float*)samples;
		count *= 2;

		//Process
		while (count > 0) {
			//Determine number of writable objects
			int writable = std::min(count, RAPTOR_BUFFER_SIZE * 2);

			//Transform to buffer
			short* endpoint = tempBuffer;
			for (int i = 0; i < writable; i++)
				transfer(endpoint++, data++);

			//Write to file
			fwrite(tempBuffer, sizeof(short), writable, handle);

			//Update state
			count -= writable;
		}
	}

private:
	FILE* handle;
	short tempBuffer[RAPTOR_BUFFER_SIZE * 2];

	inline void transfer(short* output, float* input) {
		output[0] = (short)(std::max(-1.0f, std::min(1.0f, input[0])) * 32768);
	}

};