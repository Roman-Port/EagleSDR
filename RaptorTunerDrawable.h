#pragma once
#include "RaptorDrawable.h"
#include "RaptorFontRenderer.h"

typedef void(*raptor_tuner_callback)(unsigned int index);

class RaptorTunerDrawable : public RaptorDrawable {

	virtual int get_requested_width() override;
	virtual int get_requested_height() override;
	virtual void layout_self() override;
	virtual void draw_self() override;
	virtual void tick_self() override;
	virtual bool get_resize_allowed() override;
	virtual bool process_input(int action) override;
	virtual bool hover_enter_self(int initiatorAction) override;
	virtual void hover_leave_self() override;

public:
	RaptorTunerDrawable(int groups, unsigned int value, raptor_tuner_callback callback);

private:
	raptor_tuner_callback callback;
	RaptorFontRenderer renderer;
	RaptorFontRenderer rendererDark;
	int digits;
	int decimals;
	int digitWidth;
	int digitHeight;
	int selectedIndex;
	long value;

	char get_digit(int index);
	void update_digit(int index, int direction);
	RaptorFontRenderer* get_renderer(int index);

};