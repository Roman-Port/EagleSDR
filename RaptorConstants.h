#pragma once

const int RAPTOR_BUFFER_SIZE = 65536;
const int RAPTOR_OUTPUT_RATE = 48000;

#define RAPTOR_SCREEN_WIDTH 480
#define RAPTOR_SCREEN_HEIGHT 800

#define RAPTOR_SPECTRUM_WIDTH 480

#define RAPTOR_COLOR_BACK raptor_pixel(34, 35, 37)
#define RAPTOR_COLOR_FORE raptor_pixel(51, 54, 60)
#define RAPTOR_COLOR_BORDER raptor_pixel(69, 72, 77)
#define RAPTOR_COLOR_SELECT raptor_pixel(242, 231, 27)

#ifdef RAPTOR_LOG
#define raptor_debug_log printf
#else
#define raptor_debug_log
#endif