#include "RaptorStackedHorizDrawable.h"
#include <algorithm>

int RaptorStackedHorizDrawable::get_requested_height()
{
	int size = 0;
	RaptorDrawable* cursor = 0;
	while (enumerate_children(&cursor)) {
		size = std::max(size, cursor->get_requested_height());
	}
	return size + padding_top + padding_bottom;
}

int RaptorStackedHorizDrawable::get_requested_width()
{
	return RAPTOR_SCREEN_WIDTH;
}

int RaptorStackedHorizDrawable::stacked_container_size()
{
	return get_width() - padding_left - padding_right;
}

void RaptorStackedHorizDrawable::stacked_query(RaptorDrawable* drawable, int* requestedSize, bool* resizeAllowed)
{
	*requestedSize = drawable->get_requested_width();
	*resizeAllowed = drawable->get_resize_allowed();
}

void RaptorStackedHorizDrawable::stacked_apply(RaptorDrawable* drawable, int size, int offset)
{
	drawable->request_layout(offset + padding_left, padding_top, size, get_height() - padding_top - padding_bottom);
}
