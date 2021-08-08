#include "RaptorDisplay.h"
#include "RaptorConstants.h"
#include <malloc.h>
#include <unistd.h>

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

#include <sys/mman.h>
#include <stdio.h>
#include <iostream>

void RaptorDisplay::configure(int layerCount) {
	//Log some info
	raptor_debug_log("INFO: Creating raptor display with %i layers at %ix%i.\n", layerCount, RAPTOR_SCREEN_WIDTH, RAPTOR_SCREEN_HEIGHT);
	raptor_debug_log("INFO: Raptor display consumes %i bytes.\n", (sizeof(raptor_pixel) * RAPTOR_SCREEN_WIDTH * RAPTOR_SCREEN_HEIGHT));

	//Configure
	this->layerCount = layerCount;

	//Allocate
	frame = (raptor_pixel*)malloc(sizeof(raptor_pixel) * RAPTOR_SCREEN_WIDTH * RAPTOR_SCREEN_HEIGHT);
	layers = (RaptorLayer**)malloc(sizeof(RaptorLayer*) * layerCount);

	//Configure all layers
	for (int i = 0; i < layerCount; i++) {
		layers[i] = new RaptorLayer();
		layers[i]->configure_layer(frame);
	}
}

RaptorLayer* RaptorDisplay::get_layer(int index) {
	return layers[index];
}

void RaptorDisplay::start() {
	//Open the framebuffer file
	fileHandle = open("/dev/fb0", O_RDWR);
	if (!fileHandle) {
		throw std::runtime_error("cannot open framebuffer device.");
	}

	//Map into user memory
	displayBuffer = (char*)mmap(0, RAPTOR_SCREEN_WIDTH * RAPTOR_SCREEN_HEIGHT * 4, PROT_READ | PROT_WRITE, MAP_SHARED, fileHandle, 0);
	if ((int)displayBuffer == -1) {
		throw std::runtime_error("Failed to mmap framebuffer.");
	}

	//Zero out the frame buffer
	char* outputPtr = displayBuffer;
	for (int i = 0; i < RAPTOR_SCREEN_WIDTH * RAPTOR_SCREEN_HEIGHT; i++) {
		*outputPtr++ = 0;
		*outputPtr++ = 0;
		*outputPtr++ = 0;
		*outputPtr++ = 255;
	}

	//Send a null input to all layers to get things going
	for (int i = 0; i < layerCount; i++) {
		layers[i]->hover_enter(0);
		layers[i]->process_input(0);
	}

	//Spin up worker thread
	worker = new std::thread(run_static, this);
	key_worker = new std::thread(run_debug_key_reader, this);
}

void RaptorDisplay::run() {
	while (true) {
		//Update first active layer
		for (int i = 0; i < layerCount; i++) {
			if (layers[i]->is_layer_enabled()) {
				layers[i]->update_layer();
				break;
			}
		}

		//Convert and write into display buffer
		char* outputPtr = displayBuffer;
		for (int i = 0; i < RAPTOR_SCREEN_WIDTH * RAPTOR_SCREEN_HEIGHT; i++) {
			*outputPtr++ = frame[i].b;
			*outputPtr++ = frame[i].g;
			*outputPtr++ = frame[i].r;
			outputPtr++;
		}

		//Wait
		usleep(1000000 / 30);
	}
}

void RaptorDisplay::process_input(int key) {
	//Send to first active layer
	for (int i = 0; i < layerCount; i++) {
		if (layers[i]->is_layer_enabled()) {
			layers[i]->process_input(key);
			break;
		}
	}
}

void RaptorDisplay::run_debug_key_reader(RaptorDisplay* display) {
	//Configure for reading, TEMPORARY
	//This disables buffering
	system("/bin/stty raw");

	//Show the debug keyboard
	printf("======[ DEBUG INPUT LAYOUT ]=====\n");
	printf("|                               |\n");
	printf("|     [W]       ~               |\n");
	printf("| [A]     [D]   ~   ['] [ENTER] |\n");
	printf("|     [S]       ~               |\n");
	printf("|                               |\n");
	printf("=================================\n");

	//Enter loop
	int key;
	while (true) {
		//Read, waits for a key
		key = getchar();

		//Remap the key
		switch (key) {
		case 13: display->process_input(RAPTOR_INPUT_ENTER); break;
		case 39: display->process_input(RAPTOR_INPUT_BACK); break;
		case 119: display->process_input(RAPTOR_INPUT_UP); break;
		case 115: display->process_input(RAPTOR_INPUT_DOWN); break;
		case 97: display->process_input(RAPTOR_INPUT_LEFT); break;
		case 100: display->process_input(RAPTOR_INPUT_RIGHT); break;
		}
	}
}