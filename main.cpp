#include <cstdio>

#include "RaptorRadio.h"
#include "RaptorDisplay.h"
#include "RaptorSystemStatusBarDrawable.h"
#include "RaptorQuickBarDrawable.h"
#include "RaptorWaterfallDrawable.h"
#include "RaptorSpectrumDrawable.h"
#include "RaptorRdsBarDrawable.h"
#include "RaptorSystemBarController.h"
#include "RaptorSysIconBattery.h"
#include "RaptorSysIconSignal.h"

int main()
{
    //Log
    printf("Starting RaptorSDR...\n");

    //Set up system bar
    RaptorSystemBarController::init();
    RaptorSystemBarController::add_icon(new RaptorSysIconSignal(), 999);
    //RaptorSystemBarController::add_icon(new RaptorSysIconBattery(), 998);

    //Set up display
    RaptorDisplay display;
    display.configure(3);

    //Set up radio
    char* radioError;
    RaptorRadio::initialize(&radioError);
    RaptorRadio::set_demodulator(0);

    //Set up main view
    RaptorLayer* mainView = display.get_layer(0);
    mainView->enable_layer();
    mainView->set_background_color(raptor_pixel(255, 0, 0), 1);
    mainView->add_child(new RaptorSystemStatusBarDrawable());
    mainView->add_child(new RaptorRdsBarDrawable());
    mainView->add_child(new RaptorSpectrumDrawable(RaptorRadio::get_spectrum_main()));
    mainView->add_child(new RaptorWaterfallDrawable(RaptorRadio::get_spectrum_main()));
    mainView->add_child(new RaptorSpectrumDrawable(RaptorRadio::get_spectrum_mpx()));
    mainView->add_child(new RaptorQuickBarDrawable());

    //Start display and run radio (forever)
    display.start();
    RaptorRadio::run();

    return 0;
}