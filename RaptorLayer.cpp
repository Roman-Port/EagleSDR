#include "RaptorLayer.h"

void RaptorLayer::configure_layer(raptor_pixel* display) {
	this->display = display;
	layerEnabled = false;
	request_layout(0, 0, RAPTOR_SCREEN_WIDTH, RAPTOR_SCREEN_HEIGHT);
}

void RaptorLayer::update_layer() {
	//If we're not enabled, ignore
	if (!layerEnabled)
		return;

	//Check if a layout is requested
	if(is_layout_refresh_requested())
		layout(display, RAPTOR_SCREEN_WIDTH, RAPTOR_SCREEN_HEIGHT);

	//Tick first
	tick();

	//Draw if needed
	draw();
}

void RaptorLayer::enable_layer() {
	layerEnabled = true;
}

void RaptorLayer::disable_layer() {
	layerEnabled = false;
}

bool RaptorLayer::is_layer_enabled() {
	return layerEnabled;
}