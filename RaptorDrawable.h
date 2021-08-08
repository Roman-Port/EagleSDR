#pragma once

#include "RaptorPixel.h"
#include "RaptorConstants.h"
#include "RaptorLayoutMode.h"

#define RAPTOR_DRAWABLE_MAX_CHILDREN 16

//describe() flags
#define RAPTOR_DRAWABLE_FLAG_SELECTABLE 1

//process_input() keys
#define RAPTOR_INPUT_ENTER 1
#define RAPTOR_INPUT_BACK 2
#define RAPTOR_INPUT_UP 11
#define RAPTOR_INPUT_DOWN 12
#define RAPTOR_INPUT_LEFT 13
#define RAPTOR_INPUT_RIGHT 14

/// <summary>
/// A drawable is the baseline for anything drawn on the screen. Anything drawn should extend this class.
/// </summary>
class RaptorDrawable {

public:
	RaptorDrawable();

	/// <summary>
	/// Attaches a new child to this element, then triggers a layout and redraw
	/// </summary>
	/// <param name="drawable"></param>
	void add_child(RaptorDrawable* drawable);

	/// <summary>
	/// Here, the requested height (not the actual!) of the object is returned
	/// </summary>
	/// <returns></returns>
	virtual int get_requested_height();

	/// <summary>
	/// Here, the requested width (not the actual!) of the object is returned
	/// </summary>
	/// <returns></returns>
	virtual int get_requested_width();

	/// <summary>
	/// Returns if the object can be resized from its requested width/height
	/// </summary>
	/// <returns></returns>
	virtual bool get_resize_allowed();

	/// <summary>
	/// Returns the actual width of the object.
	/// </summary>
	/// <returns></returns>
	int get_width();

	/// <summary>
	/// Returns the actual height of the object.
	/// </summary>
	/// <returns></returns>
	int get_height();

	/// <summary>
	/// Loops through all children. This should be used as the condition of a while loop. The cursor should also be NULL on first run and will be set to the first element. Example:
	/// 
	/// RaptorDrawable* cursor = 0;
	/// while (enumerate_children(&cursor)) {
	///		//Use cursor here
	/// }
	/// </summary>
	/// <param name="cursor"></param>
	/// <returns></returns>
	bool enumerate_children(RaptorDrawable** cursor);

	/// <summary>
	/// Identical to enumerate_children, except in the backwards direction. Starts with the last child.
	/// </summary>
	/// <param name="cursor"></param>
	/// <returns></returns>
	bool enumerate_children_backwards(RaptorDrawable** cursor);

	/// <summary>
	/// Requests that an element goes to a certain location. Most likely will be honored, but not guaranteed. Returns if the request was honored.
	/// </summary>
	/// <param name="offsetX"></param>
	/// <param name="offsetY"></param>
	/// <param name="width"></param>
	/// <param name="height"></param>
	bool request_layout(int offsetX, int offsetY, int width, int height);

	/// <summary>
	/// Flags this element (and children) to be redrawn next frame
	/// </summary>
	void invalidate();

	/// <summary>
	/// Makes layout of this object controlled by its parent.
	/// </summary>
	void set_layoutmode_auto();

	/// <summary>
	/// Makes layout of this object forced to the parameters specified, ignoring the parent.
	/// </summary>
	void set_layoutmode_absolute(int top, int left, int bottom, int right, int width, int height);

	/// <summary>
	/// Sets the background color of this object.
	/// </summary>
	/// <param name="color">Fill color.</param>
	/// <param name="alpha">Transparency. 1 is full, 0 is hidden.</param>
	virtual void set_background_color(raptor_pixel color, float alpha = 1);

	/// <summary>
	/// Turns off the background of this object.
	/// </summary>
	virtual void set_background_off();

	/// <summary>
	/// Sends input to the object. Returns true if it was handled, or false if it did nothing.
	/// </summary>
	/// <param name="action"></param>
	/// <returns></returns>
	virtual bool process_input(int action);

	/// <summary>
	/// Sets flags for each flag that the object supports. MUST call base function every time in overriden class.
	/// </summary>
	/// <param name="flags"></param>
	virtual void describe(int* flags);

	/// <summary>
	/// Returns if we're being hovered over.
	/// </summary>
	/// <returns></returns>
	bool get_is_hovering();

	/// <summary>
	/// Called when the cursor attempts to hover over this element. Returns true if it was handled, otherwise false.
	/// </summary>
	/// <returns></returns>
	bool hover_enter(int initiatorAction);

	/// <summary>
	/// Called when the cursor leaves this element. Only called if the element is hovered.
	/// </summary>
	void hover_leave();

	/// <summary>
	/// Returns the current background color
	/// </summary>
	/// <returns></returns>
	raptor_pixel get_background_color();

protected:

	/// <summary>
	/// Here, you MUST layout all children by calling their request_layout()
	/// </summary>
	virtual void layout_self() = 0;

	/// <summary>
	/// Called when we should draw ourself. Children are drawn after this step.
	/// </summary>
	virtual void draw_self() = 0;

	/// <summary>
	/// Called each frame, every time, before drawing. Called even if we're not invalidated.
	/// </summary>
	virtual void tick_self() = 0;

	/// <summary>
	/// Called when the cursor attempts to hover over this element. Returns true if it was handled, otherwise false. ONLY TO BE USED INTERNALLY.
	/// </summary>
	/// <returns></returns>
	virtual bool hover_enter_self(int initiatorAction);

	/// <summary>
	/// Called when the cursor leaves this element. Only called if the element is hovered. ONLY TO BE USED INTERNALLY.
	/// </summary>
	virtual void hover_leave_self();

	/// <summary>
	/// Get the offset pixel from the top left of the object.
	/// </summary>
	/// <param name="offsetX"></param>
	/// <param name="offsetY"></param>
	/// <returns></returns>
	virtual raptor_pixel* get_pixel(int offsetX, int offsetY);

	/// <summary>
	/// Fills a region with a color.
	/// </summary>
	/// <param name="pixel"></param>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <param name="width"></param>
	/// <param name="height"></param>
	void fill(raptor_pixel pixel, int x, int y, int width, int height);

	/// <summary>
	/// Fills a region with a color.
	/// </summary>
	/// <param name="pixel"></param>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <param name="width"></param>
	/// <param name="height"></param>
	void fill(raptor_pixel* ptr, raptor_pixel color, int width, int height);

	/// <summary>
	/// Fills a region with a color, with transparency. Slow.
	/// </summary>
	/// <param name="pixel"></param>
	/// <param name="alpha"></param>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <param name="width"></param>
	/// <param name="height"></param>
	void fill_blend(raptor_pixel pixel, float alpha, int x, int y, int width, int height);

	/// <summary>
	/// Blends a pixel with another color. Slow.
	/// </summary>
	/// <param name="pixel"></param>
	/// <param name="color"></param>
	/// <param name="alpha"></param>
	void blend_pixel(raptor_pixel* pixel, raptor_pixel color, float alpha);

	/// <summary>
	/// Draws an outline.
	/// </summary>
	/// <param name="pixel"></param>
	/// <param name="color"></param>
	/// <param name="width"></param>
	/// <param name="height"></param>
	/// <param name="thickness"></param>
	void helper_draw_outline(raptor_pixel* pixel, raptor_pixel color, int width, int height, int thickness);

	/// <summary>
	/// Helper to draw border.
	/// </summary>
	/// <param name="size"></param>
	/// <param name="color"></param>
	void helper_draw_border_top(int size, raptor_pixel color);

	/// <summary>
	/// Helper to draw border.
	/// </summary>
	/// <param name="size"></param>
	/// <param name="color"></param>
	void helper_draw_border_bottom(int size, raptor_pixel color);

	/// <summary>
	/// Helper to draw border.
	/// </summary>
	/// <param name="size"></param>
	/// <param name="color"></param>
	void helper_draw_border_left(int size, raptor_pixel color);

	/// <summary>
	/// Helper to draw border.
	/// </summary>
	/// <param name="size"></param>
	/// <param name="color"></param>
	void helper_draw_border_right(int size, raptor_pixel color);

	/// <summary>
	/// Layout this and children. ONLY TO BE USED INTERNALLY.
	/// </summary>
	void layout(raptor_pixel* ctx, int containerWidth, int containerHeight);

	/// <summary>
	/// Ticks this and children. ONLY TO BE USED INTERNALLY.
	/// </summary>
	void tick();

	/// <summary>
	/// Redraws this and children. ONLY TO BE USED INTERNALLY.
	/// </summary>
	void draw();

	/// <summary>
	/// Flags the layout to be reset during the next tick.
	/// </summary>
	void invalidate_layout();

	/// <summary>
	/// Returns true if a refresh of the layout is requested by this or any of it's children.
	/// </summary>
	/// <returns></returns>
	bool is_layout_refresh_requested();

private:
	//Children
	RaptorDrawable* children[RAPTOR_DRAWABLE_MAX_CHILDREN];
	int childrenCount;

	raptor_pixel* ctx;
	int width;
	int height;

	//Misc
	bool isHovering;

	//Background
	float backgroundAlpha;
	raptor_pixel backgroundColor;

	//Layout
	bool isInvalidated;
	bool layoutInvalidated;
	RaptorLayoutMode layoutMode;

	//For AUTO layout mode only
	int layoutAutoX;
	int layoutAutoY;
	int layoutAutoWidth;
	int layoutAutoHeight;

	//For ABSOLUTE layout mode only
	int layoutAbsoluteTop;
	int layoutAbsoluteBottom;
	int layoutAbsoluteLeft;
	int layoutAbsoluteRight;
	int layoutAbsoluteWidth;
	int layoutAbsoluteHeight;

};