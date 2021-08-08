#pragma once

#include "RaptorDrawable.h"
#include "RaptorSpectrumController.h"
#include "RaptorPixel.h"

class RaptorSpectrumDrawable : public RaptorDrawable {


	virtual int get_requested_height() override;
	virtual int get_requested_width() override;
	virtual void layout_self() override;
	virtual void draw_self() override;
	virtual void tick_self() override;

public:
	RaptorSpectrumDrawable(RaptorSpectrumController* spectrum);

private:
	RaptorSpectrumController* spectrum;
	float bufferRaw[RAPTOR_SPECTRUM_WIDTH];
	int bufferPixel[RAPTOR_SPECTRUM_WIDTH];
	raptor_pixel* gradientBackground;
	raptor_pixel* gradientForeground;

	raptor_pixel** gridlineV;
	raptor_pixel** gridlineH;
	int gridlineCountV;
	int gridlineCountH;

	void reconfigure_grid();
	void reconfigure_grid_v();
	void reconfigure_grid_h();

	static int util_find_pixel_points(float start, float end, float increment, float width, int* outPixel, float* outValue);

	const raptor_pixel GRADIENT_BACK_A = raptor_pixel(52, 83, 117);
	const raptor_pixel GRADIENT_BACK_B = raptor_pixel(0, 0, 20);
	const raptor_pixel GRADIENT_FORE_A = raptor_pixel(112, 180, 255);
	const raptor_pixel GRADIENT_FORE_B = raptor_pixel(0, 0, 80);

};