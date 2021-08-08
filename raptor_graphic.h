#pragma once

#include "RaptorPixel.h"
#include "RaptorConstants.h"
#include <cstring>

struct raptor_graphic {

	int width;
	int height;
	int bpp;
	const char* payload;

	void draw(raptor_pixel* ptr) {
		//Check what mode the image was encoded in
		if (bpp == 1) {
			//Get pointer to palette
			raptor_pixel* palette = (raptor_pixel*)&payload[0];

			//Unpack image
			unsigned char* data = (unsigned char*)&payload[6];
			int bitIndex = 0;
			for (int y = 0; y < height; y++) {
				//Convert pixels
				for (int x = 0; x < width; x++) {
					//Paint
					ptr[x] = palette[(*data >> bitIndex++) & 1];

					//Advance if needed
					if (bitIndex == 8) {
						data++;
						bitIndex = 0;
					}
				}

				//Offset dst pointer
				ptr += RAPTOR_SCREEN_WIDTH;
			}
		}
		if (bpp == 8) {
			//Get the number of colors in the palette
			int colors = (int)payload[0];

			//Get pointer to palette
			raptor_pixel* palette = (raptor_pixel*)&payload[1];

			//Unpack image
			unsigned char* data = (unsigned char*)&payload[1 + (colors * 3)];
			for (int y = 0; y < height; y++) {
				//Convert pixels
				for (int x = 0; x < width; x++)
					ptr[x] = palette[*data++];

				//Offset dst pointer
				ptr += RAPTOR_SCREEN_WIDTH;
			}
		}
		
	}

};