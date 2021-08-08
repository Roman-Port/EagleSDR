#pragma once

#include "RaptorLayer.h"
#include "RaptorPixel.h"
#include <thread>

class RaptorDisplay {

public:
	void configure(int layerCount);
	RaptorLayer* get_layer(int index);
	void start();

private:
	raptor_pixel* frame;
	RaptorLayer** layers;
	int layerCount;
	std::thread* worker;
	int fileHandle;
	char* displayBuffer;

	void process_input(int key);

	static void run_static(RaptorDisplay* display) {
		display->run();
	}

	std::thread* key_worker;
	static void run_debug_key_reader(RaptorDisplay* display);

	void run();

};