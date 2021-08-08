#pragma once

#include "RaptorDrawable.h"

class RaptorMenuHelper {

public:
	RaptorMenuHelper();
	void menu_next(RaptorDrawable* self);
	void menu_previous(RaptorDrawable* self);
	void menu_reset();
	RaptorDrawable* selected;

private:
	void menu_select(RaptorDrawable* newSelection, int initiatorAction);
	bool menu_try_select(RaptorDrawable* newSelection, int initiatorAction);

};