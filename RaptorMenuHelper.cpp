#include "RaptorMenuHelper.h"

RaptorMenuHelper::RaptorMenuHelper() {
	selected = 0;
}

void RaptorMenuHelper::menu_next(RaptorDrawable* self) {
	//Forward-search for the element after the selected one
	bool selectedFound = false;
	RaptorDrawable* cursor = 0;
	while (self->enumerate_children(&cursor)) {
		//Find the first element
		if (cursor == selected && !selectedFound) {
			selectedFound = true;
			continue;
		}

		//Try to select
		if (selectedFound && menu_try_select(cursor, RAPTOR_INPUT_RIGHT))
			return;
	}

	//Wrap around
	cursor = 0;
	while (self->enumerate_children(&cursor)) {
		//Try to select
		if (menu_try_select(cursor, RAPTOR_INPUT_RIGHT))
			return;
	}

	//Nothing to select!
	cursor = 0;
}

void RaptorMenuHelper::menu_previous(RaptorDrawable* self) {
	//Forward-search for the element after the selected one
	bool selectedFound = false;
	RaptorDrawable* cursor = 0;
	while (self->enumerate_children_backwards(&cursor)) {
		//Find the first element
		if (cursor == selected && !selectedFound) {
			selectedFound = true;
			continue;
		}

		//Try to select
		if (selectedFound && menu_try_select(cursor, RAPTOR_INPUT_LEFT))
			return;
	}

	//Wrap around
	cursor = 0;
	while (self->enumerate_children_backwards(&cursor)) {
		//Try to select
		if (menu_try_select(cursor, RAPTOR_INPUT_LEFT))
			return;
	}

	//Nothing to select!
	cursor = 0;
}

void RaptorMenuHelper::menu_reset() {
	//Deselect the current object, if any
	if (selected != 0)
		selected->hover_leave();

	//Set to null
	selected = 0;
}

bool RaptorMenuHelper::menu_try_select(RaptorDrawable* selection, int initiatorAction) {
	if (selection != 0 && selection->hover_enter(initiatorAction)) {
		menu_select(selection, initiatorAction);
		return true;
	}
	else {
		return false;
	}
}

void RaptorMenuHelper::menu_select(RaptorDrawable* newSelected, int initiatorAction) {
	//Deselect the current object, if any
	if (selected != 0)
		selected->hover_leave();

	//Set the selection
	selected = newSelected;
}