#pragma once

struct raptor_font {

	int height;
	const int width[256];
	const int offset[256];
	const char* charset[256];

};