#include "RaptorSpectrumDrawable.h"
#include "RaptorConstants.h"
#include <cstring>

#define RAPTOR_SPECTRUM_DRAWABLE_PADDING_HEIGHT 0

RaptorSpectrumDrawable::RaptorSpectrumDrawable(RaptorSpectrumController* spectrum) {
	this->spectrum = spectrum;
}

int RaptorSpectrumDrawable::get_requested_height()
{
	return 500;
}

int RaptorSpectrumDrawable::get_requested_width()
{
	return RAPTOR_SPECTRUM_WIDTH;
}

void RaptorSpectrumDrawable::layout_self()
{
	//Allocate new gradient table
	gradientBackground = (raptor_pixel*)malloc(get_height() * sizeof(raptor_pixel));
	gradientForeground = (raptor_pixel*)malloc(get_height() * sizeof(raptor_pixel));

	//Now, mix colors into this
	for (int i = 0; i < get_height(); i++) {
		//Calculate basic values
		float ratio = (float)i / get_height();
		float inverse = 1 - ratio;

		//Apply to background gradient
		gradientBackground[i].r = (int)((GRADIENT_BACK_A.r * inverse) + (GRADIENT_BACK_B.r * ratio));
		gradientBackground[i].g = (int)((GRADIENT_BACK_A.g * inverse) + (GRADIENT_BACK_B.g * ratio));
		gradientBackground[i].b = (int)((GRADIENT_BACK_A.b * inverse) + (GRADIENT_BACK_B.b * ratio));

		//Apply to foreground gradient
		gradientForeground[i].r = (int)((GRADIENT_FORE_A.r * inverse) + (GRADIENT_FORE_B.r * ratio));
		gradientForeground[i].g = (int)((GRADIENT_FORE_A.g * inverse) + (GRADIENT_FORE_B.g * ratio));
		gradientForeground[i].b = (int)((GRADIENT_FORE_A.b * inverse) + (GRADIENT_FORE_B.b * ratio));
	}

	//Reconfigure grid grid
	reconfigure_grid();
}

void RaptorSpectrumDrawable::tick_self()
{
	invalidate();
}

void RaptorSpectrumDrawable::draw_self()
{
	//First, capture a frame from the spectrum
	spectrum->read(bufferRaw);

	//Now, convert it to pixel space
	int pixelScale = get_height() - 1;
	for (int i = 0; i < RAPTOR_SPECTRUM_WIDTH; i++)
		bufferPixel[i] = (int)(bufferRaw[i] * pixelScale);

	//Fill background for each column
	raptor_pixel* pixel;
	for (int x = 0; x < RAPTOR_SPECTRUM_WIDTH; x++) {
		//Get this pixel
		pixel = get_pixel(x, 0);

		//Fill unshaded
		for (int y = 0; y < bufferPixel[x]; y++) {
			*pixel = gradientBackground[y];
			pixel += RAPTOR_SCREEN_WIDTH;
		}

		//Fill shaded
		for (int y = bufferPixel[x]; y < get_height(); y++) {
			*pixel = gradientForeground[y];
			pixel += RAPTOR_SCREEN_WIDTH;
		}
	}

	//To begin drawing lines, draw the first line like normal
	*get_pixel(0, bufferPixel[0]) = raptor_pixel(255, 255, 255);

	//Draw lines for canvas
	int current;
	int previous = bufferPixel[0];
	int direction;
	for (int i = 1; i < RAPTOR_SPECTRUM_WIDTH; i++) {
		//Read value
		current = bufferPixel[i];

		//Get this pixel
		pixel = get_pixel(i, current);

		//Set
		*pixel = raptor_pixel(255, 255, 255);

		//Connect to previous
		direction = current < previous ? 1 : -1;
		while (current != previous) {
			current += direction;
			pixel += (RAPTOR_SCREEN_WIDTH * direction);
			*pixel = raptor_pixel(180, 180, 180);
		}

		//Update state
		previous = bufferPixel[i];
	}

	//Draw vertical lines
	for (int i = 0; i < gridlineCountV; i++) {
		//Get base pixel
		pixel = gridlineV[i];

		//Draw from top to bottom
		for (int y = 0; y < get_height(); y++) {
			*pixel = raptor_pixel(180, 180, 180);
			pixel += RAPTOR_SCREEN_WIDTH;
		}
	}

	//Draw horizontal lines
	for (int i = 0; i < gridlineCountH; i++) {
		//Get base pixel
		pixel = gridlineH[i];

		//Draw from left to right
		memset(pixel, 180, get_width() * sizeof(raptor_pixel));
	}
}

void RaptorSpectrumDrawable::reconfigure_grid() {
	reconfigure_grid_h();
	reconfigure_grid_v();
}

void RaptorSpectrumDrawable::reconfigure_grid_v() {
	gridlineCountV = 0;
}

void RaptorSpectrumDrawable::reconfigure_grid_h() {
	//Calculate the number of points we can nicely display on screen
	int maxPoints = (get_height() - RAPTOR_SPECTRUM_DRAWABLE_PADDING_HEIGHT) / 12;

	//Get the actual increment
	int increment = 5;
	while ((spectrum->range / increment) > maxPoints)
		increment *= 2;

	//Query size of pixel points
	int points = util_find_pixel_points(spectrum->offset, spectrum->offset + spectrum->range, increment, get_height() - RAPTOR_SPECTRUM_DRAWABLE_PADDING_HEIGHT, 0, 0);

	//Allocate temporary buffers
	int pointPixels[points];
	float pointValues[points];

	//Query real points
	util_find_pixel_points(spectrum->offset, spectrum->offset + spectrum->range, increment, get_height() - RAPTOR_SPECTRUM_DRAWABLE_PADDING_HEIGHT, pointPixels, pointValues);

	//Allocate points
	gridlineH = (raptor_pixel**)malloc(sizeof(raptor_pixel*) * points);
	gridlineCountH = points;

	//Begin filling points
	for (int i = 0; i < points; i++)
		gridlineH[i] = get_pixel(0, pointPixels[i]);

	//Draw labels
	//...
}

int RaptorSpectrumDrawable::util_find_pixel_points(float start, float end, float increment, float width, int* outPixel, float* outValue) {
	//Get first visible
	float first = std::ceil(start / increment) * increment;

	//Calculate numbers per pixel
	float scale = width / (end - start);

	//Begin calculating pixels
	int count = 0;
	for (float i = first; i <= end; i += increment) {
		//Only if the arrays aren't null, push the result. They may be null when querying the count.
		if (outPixel != 0 && outValue != 0) {
			outValue[count] = i;
			outPixel[count] = (int)((i - start) * scale);
		}

		//Update count
		count++;
	}

	return count;
}