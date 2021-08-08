#pragma once

#include <volk/volk.h>

class RaptorFirFilterComplex {

public:
	RaptorFirFilterComplex(lv_32fc_t* coeffs, int coeffsCount, int decimation) {
		//Create aligned buffers
		size_t alignment = volk_get_alignment();
		coeffsBufferPtr = (lv_32fc_t*)volk_malloc(coeffsCount * sizeof(lv_32fc_t), alignment);
		insampBufferPtr = (lv_32fc_t*)volk_malloc(coeffsCount * 2 * sizeof(lv_32fc_t), alignment);
		tempBufferPtr = (lv_32fc_t*)volk_malloc(1 * sizeof(lv_32fc_t), alignment);

		//Apply setings
		insampBufferPtrOffset = &insampBufferPtr[coeffsCount];
		taps = coeffsCount;
		this->decimation = decimation;
		decimationIndex = 0;
		offset = 0;

		//Copy taps
		for (int i = 0; i < coeffsCount; i++)
			coeffsBufferPtr[i] = coeffs[i];
	}

	inline int Process(lv_32fc_t* input, lv_32fc_t* output, int count, int channels) {
		int read = 0;
		for (int i = 0; i < count; i++)
		{
			//Write to both the real position as well as an offset value
			this->insampBufferPtr[this->offset] = *input;
			this->insampBufferPtrOffset[this->offset++] = *input;
			input += channels;
			this->offset %= this->taps;

			//Process (if needed)
			if (this->decimationIndex++ == 0)
			{
				volk_32fc_x2_dot_prod_32fc(tempBufferPtr, &insampBufferPtr[offset], coeffsBufferPtr, taps);
				output[read++ * channels] = tempBufferPtr[0];
			}
			this->decimationIndex %= this->decimation;
		}
		return read;
	}

private:
	lv_32fc_t* coeffsBufferPtr;
	lv_32fc_t* insampBufferPtr;
	lv_32fc_t* insampBufferPtrOffset;
	lv_32fc_t* tempBufferPtr; //used to keep VOLK aligned
	int taps;
	int decimation;
	int decimationIndex;
	int offset;

};