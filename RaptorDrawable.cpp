#include "RaptorDrawable.h"
#include <stdexcept>
#include <cstring>

RaptorDrawable::RaptorDrawable() {
	isInvalidated = true;
	layoutInvalidated = true;
	childrenCount = 0;
	layoutAutoX = 0;
	layoutAutoY = 0;
	layoutAutoWidth = 0;
	layoutAutoHeight = 0;
	layoutMode = RaptorLayoutMode::AUTO;
	backgroundAlpha = 0;
	isHovering = false;
}

void RaptorDrawable::add_child(RaptorDrawable* drawable) {
	//Make sure we have space
	if (childrenCount == RAPTOR_DRAWABLE_MAX_CHILDREN)
		throw std::runtime_error("Attempted to add child to a full drawable. Raising the max children may solve the problem.");

	//Add
	children[childrenCount++] = drawable;

	//Flag for resetting the layout
	invalidate_layout();
}

bool RaptorDrawable::get_resize_allowed() {
	//By default, allow it unless a child says otherwise
	bool allowed = true;
	RaptorDrawable* cursor = 0;
	while (enumerate_children(&cursor))
		allowed = allowed && cursor->get_resize_allowed();
	return allowed;
}

int RaptorDrawable::get_width() {
	return width;
}

int RaptorDrawable::get_height() {
	return height;
}

bool RaptorDrawable::enumerate_children(RaptorDrawable** cursor) {
	//Determine where the cursor is and should be pointing in index space
	int pointingIndex = 0;
	if (*cursor != 0)
	{
		//Cursor IS initialized; Loop through and find a matching one
		while (children[pointingIndex] != *cursor) {
			//Update
			pointingIndex++;

			//Safety
			if (pointingIndex > RAPTOR_DRAWABLE_MAX_CHILDREN)
				throw std::runtime_error("Drawable enumeration failed: the referenced cursor wasn't valid. Did children change or was the cursor not initialized to 0?");
		}

		//Advance cursor
		pointingIndex++;
	}

	//Set the new cursor location
	*cursor = children[pointingIndex];

	//Determine if it's valid
	return pointingIndex < childrenCount;
}

bool RaptorDrawable::enumerate_children_backwards(RaptorDrawable** cursor) {
	//Determine where the cursor is and should be pointing in index space
	int pointingIndex = childrenCount - 1;
	if (*cursor != 0)
	{
		//Cursor IS initialized; Loop through and find a matching one
		while (children[pointingIndex] != *cursor) {
			//Update
			pointingIndex--;

			//Safety
			if (pointingIndex < 0)
				throw std::runtime_error("Drawable enumeration failed: the referenced cursor wasn't valid. Did children change or was the cursor not initialized to 0?");
		}

		//Advance cursor
		pointingIndex--;
	}

	//Set the new cursor location
	if (pointingIndex >= 0) {
		*cursor = children[pointingIndex];
		return true;
	}
	else {
		return false;
	}
}

void RaptorDrawable::layout(raptor_pixel* containerCtx, int containerWidth, int containerHeight) {
	//From the layout mode, determine the size/position of this object
	switch (layoutMode) {
		case RaptorLayoutMode::AUTO:
			//Allow the parent to specify where we should be placed
			ctx = raptor_pixel::get_offset_pixel(containerCtx, layoutAutoX, layoutAutoY);
			width = layoutAutoWidth;
			height = layoutAutoHeight;
			break;
		case RaptorLayoutMode::ABSOLUTE:
			//Overrride the automatic placement and use user parameters
			throw std::runtime_error("TODO");
			break;
		default:
			//Invalid
			throw std::runtime_error("Unknown layout mode specified.");
			break;
	}

	//Run self layout
	layout_self();

	//Loop through and apply to children
	RaptorDrawable* cursor = 0;
	while (enumerate_children(&cursor)) {
		cursor->layout(ctx, width, height);
	}

	//Flag for redrawing
	invalidate();

	//Reset state
	layoutInvalidated = false;
}

void RaptorDrawable::invalidate() {
	isInvalidated = true;
}

raptor_pixel* RaptorDrawable::get_pixel(int offsetX, int offsetY) {
	return raptor_pixel::get_offset_pixel(ctx, offsetX, offsetY);
}

void RaptorDrawable::tick() {
	//First, tick ourselves
	tick_self();

	//Now, loop through and tick children
	RaptorDrawable* cursor = 0;
	while (enumerate_children(&cursor)) {
		cursor->tick();
	}
}

void RaptorDrawable::draw() {
	//If we're pending redrawing, perofmr it on ourselves
	if (isInvalidated) {
		//Update the invalidated flag in case we set it again
		isInvalidated = false;

		//Draw the background color, if needed
		if (backgroundAlpha == 1)
			fill(backgroundColor, 0, 0, get_width(), get_height());
		else if (backgroundAlpha != 0)
			fill_blend(backgroundColor, backgroundAlpha, 0, 0, get_width(), get_height());

		//Redraw ourselves
		draw_self();
	}

	//Loop through and redraw all children
	RaptorDrawable* cursor = 0;
	while (enumerate_children(&cursor)) {
		cursor->draw();
	}
}

bool RaptorDrawable::request_layout(int offsetX, int offsetY, int width, int height) {
	layoutAutoX = offsetX;
	layoutAutoY = offsetY;
	layoutAutoWidth = width;
	layoutAutoHeight = height;
	return layoutMode == RaptorLayoutMode::AUTO;
}

void RaptorDrawable::set_layoutmode_auto() {
	layoutMode = RaptorLayoutMode::AUTO;
	invalidate_layout();
}

void RaptorDrawable::set_layoutmode_absolute(int top, int left, int bottom, int right, int width, int height) {
	layoutAbsoluteTop = top;
	layoutAbsoluteLeft = left;
	layoutAbsoluteBottom = bottom;
	layoutAbsoluteRight = right;
	layoutAbsoluteWidth = width;
	layoutAbsoluteHeight = height;
	layoutMode = RaptorLayoutMode::ABSOLUTE;
	invalidate_layout();
}

void RaptorDrawable::set_background_color(raptor_pixel color, float alpha) {
	backgroundColor = color;
	backgroundAlpha = alpha;
}

void RaptorDrawable::set_background_off() {
	backgroundAlpha = 0;
}

void RaptorDrawable::fill(raptor_pixel pixel, int offsetX, int offsetY, int width, int height) {
	fill(get_pixel(offsetX, offsetY), pixel, width, height);
}

void RaptorDrawable::fill(raptor_pixel* ptr, raptor_pixel color, int width, int height) {
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++)
			ptr[x] = color;
		ptr += RAPTOR_SCREEN_WIDTH;
	}
}

void RaptorDrawable::fill_blend(raptor_pixel pixel, float alpha, int offsetX, int offsetY, int width, int height) {
	for (int y = 0; y < height; y++) {
		raptor_pixel* ptr = get_pixel(offsetX, offsetY + y);
		for (int x = 0; x < width; x++)
			blend_pixel(&ptr[x], pixel, alpha);
	}
}

void RaptorDrawable::blend_pixel(raptor_pixel* pixel, raptor_pixel color, float alpha) {
	float inverseAlpha = 1 - alpha;
	pixel->r = (unsigned char)((pixel->r * inverseAlpha) + (color.r * alpha));
	pixel->g = (unsigned char)((pixel->g * inverseAlpha) + (color.g * alpha));
	pixel->b = (unsigned char)((pixel->b * inverseAlpha) + (color.b * alpha));
}

void RaptorDrawable::helper_draw_outline(raptor_pixel* pixel, raptor_pixel color, int width, int height, int thickness) {
	fill(pixel, color, width, thickness); //top
	fill(pixel + ((height - thickness) * RAPTOR_SCREEN_WIDTH), color, width, thickness); //bottom
	fill(pixel, color, thickness, height); //left
	fill(pixel + (width - thickness), color, thickness, height); //right
}

void RaptorDrawable::invalidate_layout() {
	layoutInvalidated = true;
}

bool RaptorDrawable::is_layout_refresh_requested() {
	//Loop through and check children
	RaptorDrawable* cursor = 0;
	while (enumerate_children(&cursor)) {
		layoutInvalidated = layoutInvalidated || cursor->is_layout_refresh_requested();
	}

	return layoutInvalidated;
}

bool RaptorDrawable::process_input(int action) {
	//By default, simply try to proxy it to the first child we can find that'll handle it
	RaptorDrawable* cursor = 0;
	while (enumerate_children(&cursor)) {
		if (cursor->process_input(action))
			return true;
	}

	return false;
}

void RaptorDrawable::describe(int* flags) {
	//Do nothing with the base object...
}

bool RaptorDrawable::hover_enter(int initiatorAction) {
	isHovering = hover_enter_self(initiatorAction);
	return isHovering;
}

void RaptorDrawable::hover_leave() {
	isHovering = false;
	hover_leave_self();
}

bool RaptorDrawable::hover_enter_self(int initiatorAction) {
	//By default, simply try to proxy it to the first child we can find that'll handle it
	RaptorDrawable* cursor = 0;
	while (enumerate_children(&cursor)) {
		if (cursor->hover_enter(initiatorAction))
			return true;
	}

	return false;
}

void RaptorDrawable::hover_leave_self() {
	//By default, simply try to proxy it to the first child we can find that'll handle it
	RaptorDrawable* cursor = 0;
	while (enumerate_children(&cursor)) {
		if (cursor->isHovering) {
			cursor->hover_leave();
			return;
		}
	}

	//Failed to find it!
	throw std::runtime_error("Attempted to leave hovering on drawable, but none of the children are currently being hovered! This shouldn't be possible.");
}

bool RaptorDrawable::get_is_hovering() {
	return isHovering;
}

void RaptorDrawable::helper_draw_border_top(int size, raptor_pixel color) {
	raptor_pixel* pixel = get_pixel(0, 0);
	int width = get_width();
	for (int y = 0; y < size; y++) {
		for (int x = 0; x < width; x++)
			pixel[x] = color;
		pixel += RAPTOR_SCREEN_WIDTH;
	}
}

void RaptorDrawable::helper_draw_border_bottom(int size, raptor_pixel color) {
	raptor_pixel* pixel = get_pixel(0, get_height() - 1);
	int width = get_width();
	for (int y = 0; y < size; y++) {
		for (int x = 0; x < width; x++)
			pixel[x] = color;
		pixel -= RAPTOR_SCREEN_WIDTH;
	}
}

void RaptorDrawable::helper_draw_border_left(int size, raptor_pixel color) {
	raptor_pixel* pixel = get_pixel(0, 0);
	int height = get_height();
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < size; x++)
			pixel[x] = color;
		pixel += RAPTOR_SCREEN_WIDTH;
	}
}

void RaptorDrawable::helper_draw_border_right(int size, raptor_pixel color) {
	raptor_pixel* pixel = get_pixel(get_width() - 1 - size, 0);
	int height = get_height();
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < size; x++)
			pixel[x] = color;
		pixel += RAPTOR_SCREEN_WIDTH;
	}
}

raptor_pixel RaptorDrawable::get_background_color() {
	return backgroundColor;
}

int RaptorDrawable::get_requested_height() {
	//By default, simply total the size of children
	int size = 0;
	RaptorDrawable* cursor = 0;
	while (enumerate_children(&cursor))
		size += cursor->get_requested_height();
	return size;
}

int RaptorDrawable::get_requested_width() {
	//By default, simply total the size of children
	int size = 0;
	RaptorDrawable* cursor = 0;
	while (enumerate_children(&cursor))
		size += cursor->get_requested_width();
	return size;
}