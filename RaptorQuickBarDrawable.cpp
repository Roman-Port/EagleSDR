#include "RaptorQuickBarDrawable.h"
#include "RaptorTextButtonDrawable.h"
#include "RaptorFontStore.h"
#include "RaptorControlLabelDrawable.h"
#include "RaptorTunerDrawable.h"
#include "RaptorSelectionDrawable.h"
#include "RaptorRadio.h"

RaptorQuickBarDrawable::RaptorQuickBarDrawable() {
	//Set values
	set_background_color(raptor_pixel(34, 35, 37));
	set_padding(10);
	set_spacing(10);

	//Add buttons
	add_child(new RaptorControlLabelDrawable("FREQUENCY", RAPTOR_COLOR_BACK, new RaptorTunerDrawable(10, 92500000, RaptorRadio::set_frequency), 5, 5));
	add_child(new RaptorControlLabelDrawable("BANDWIDTH", RAPTOR_COLOR_BACK, new RaptorTunerDrawable(6, 250000, RaptorRadio::set_bandwidth), 5, 5));
	add_child(new RaptorControlLabelDrawable("MODE", RAPTOR_COLOR_BACK, new RaptorSelectionDrawable(RaptorRadio::get_demodulator_labels(), RAPTOR_DEMODULATOR_COUNT, RaptorRadio::set_demodulator), 5, 5));
}

bool RaptorQuickBarDrawable::get_resize_allowed() {
	return false;
}