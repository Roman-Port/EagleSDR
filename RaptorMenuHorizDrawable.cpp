#include "RaptorMenuHorizDrawable.h"

RaptorMenuHorizDrawable::RaptorMenuHorizDrawable() {

}

bool RaptorMenuHorizDrawable::process_input(int input) {
	//First, attempt to send the input to the currently selected item
	if (helper.selected != 0 && helper.selected->process_input(input))
		return true;

	//Falls into our control. Check inputs now
	if (input == RAPTOR_INPUT_LEFT)
		helper.menu_previous(this);
	if (input == RAPTOR_INPUT_RIGHT)
		helper.menu_next(this);

	return true;
}

bool RaptorMenuHorizDrawable::hover_enter_self() {
	//Attempt to select the first item
	helper.menu_next(this);

	return helper.selected != 0;
}

void RaptorMenuHorizDrawable::hover_leave_self() {
	//Deselect current (if any)
	helper.menu_reset();
}