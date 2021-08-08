#pragma once
#include "RaptorDrawable.h"
#include "RaptorFontRenderer.h"

typedef void(*raptor_selection_callback)(int index);

class RaptorSelectionDrawable : public RaptorDrawable {

	virtual void layout_self() override;
	virtual void draw_self() override;
	virtual void tick_self() override;
	virtual int get_requested_width() override;
	virtual int get_requested_height() override;
	virtual bool get_resize_allowed() override;
	virtual bool process_input(int action) override;
	virtual bool hover_enter_self(int initiatorAction) override;
	virtual void hover_leave_self() override;

public:
	RaptorSelectionDrawable(char** selections, int selectionsCount, raptor_selection_callback callback);

private:
	char** selections;
	int selectionsCount;
	int index;
	raptor_pixel* iconUpPos;
	raptor_pixel* iconDownPos;
	raptor_selection_callback callback;
	RaptorFontRenderer renderer;

};