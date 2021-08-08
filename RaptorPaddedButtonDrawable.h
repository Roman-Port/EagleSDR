#pragma once

#include "RaptorDrawable.h"

class RaptorPaddedButtonDrawable : public RaptorDrawable {

	// Inherited via RaptorDrawable
	virtual int get_requested_height() override;
	virtual int get_requested_width() override;
	virtual void layout_self() override;
	virtual void draw_self() override;
	virtual void tick_self() override;
	virtual bool process_input(int action) override;
	virtual bool hover_enter_self(int invokingAction) override;
	virtual void hover_leave_self() override;

public:

	RaptorPaddedButtonDrawable();
	void set_color_border(raptor_pixel color);
	void set_color_highlighted(raptor_pixel color);

	virtual void button_pressed() = 0;

private:
	int borderWidth;
	int padding;
	raptor_pixel border_color;
	raptor_pixel highlighted_color;
	void draw_border(bool selected);

};