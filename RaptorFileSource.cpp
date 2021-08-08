#include "RaptorFileSource.h"

RaptorFileSource::RaptorFileSource(char* filename) {
	this->filename = filename;
}

bool RaptorFileSource::open() {
	//Open
	file = fopen(filename, "rb");

	//Read header
	fread(&header, sizeof(char), 44, file);

	//Apply settings
	sample_rate = *((int*)&header[24]);

	return file != 0;
}

int RaptorFileSource::read(lv_32fc_t* buffer) {
	//Read
	int read = fread(rawBuffer, sizeof(short), RAPTOR_BUFFER_SIZE * 2, file);

	//Convert samples
	float* bufferFloat = (float*)buffer;
	for (int i = 0; i < read; i++)
		bufferFloat[i] = (float)rawBuffer[i] / 32768;

	return read / 2;
}

void RaptorFileSource::close() {
	fclose(file);
}

void RaptorFileSource::set_center_freq(unsigned int freq) {

}