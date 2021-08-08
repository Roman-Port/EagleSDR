#pragma once

#include "RaptorStackedVertDrawable.h"

/// <summary>
/// Serves as a basic canvas to begin building on
/// </summary>
class RaptorLayer : public RaptorStackedVertDrawable {

public:
	void configure_layer(raptor_pixel* display);
	void update_layer();
	void enable_layer();
	void disable_layer();
	bool is_layer_enabled();

private:
	raptor_pixel* display;
	bool layerEnabled;

};