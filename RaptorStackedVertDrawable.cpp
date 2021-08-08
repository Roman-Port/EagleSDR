#include "RaptorStackedVertDrawable.h"
#include <algorithm>

int RaptorStackedVertDrawable::get_requested_height()
{
	return RAPTOR_SCREEN_HEIGHT;
}

int RaptorStackedVertDrawable::get_requested_width()
{
	int size = 0;
	RaptorDrawable* cursor = 0;
	while (enumerate_children(&cursor)) {
		size = std::max(size, cursor->get_requested_width());
	}
	return size + padding_left + padding_right;
}

int RaptorStackedVertDrawable::stacked_container_size()
{
	return get_height() - padding_top - padding_bottom;
}

void RaptorStackedVertDrawable::stacked_query(RaptorDrawable* drawable, int* requestedSize, bool* resizeAllowed)
{
	*requestedSize = drawable->get_requested_height();
	*resizeAllowed = drawable->get_resize_allowed();
}

void RaptorStackedVertDrawable::stacked_apply(RaptorDrawable* drawable, int size, int offset)
{
	drawable->request_layout(padding_left, offset + padding_top, get_width() - padding_left - padding_right, size);
}
