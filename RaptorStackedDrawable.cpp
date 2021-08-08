#include "RaptorStackedDrawable.h"
#include <algorithm>

RaptorStackedDrawable::RaptorStackedDrawable() {
	padding_left = 0;
	padding_bottom = 0;
	padding_top = 0;
	padding_right = 0;
	spacing = 0;
}

void RaptorStackedDrawable::layout_self()
{
	//Get the size of the container
	int containerSize = stacked_container_size();
	
	//We'll need to query all children to find properties about them. To do that, allocate some arrays for info
	int childrenSizes[RAPTOR_DRAWABLE_MAX_CHILDREN];
	bool childrenAllowGrowth[RAPTOR_DRAWABLE_MAX_CHILDREN];
	int childrenCount = 0;

	//Query our children
	RaptorDrawable* cursor = 0;
	while (enumerate_children(&cursor)) {
		stacked_query(cursor, &childrenSizes[childrenCount], &childrenAllowGrowth[childrenCount]);
		childrenCount++;
	}

	//Calculate the size of the spacing and subtract it from the size we have to work with
	int spacingSize = std::max(0, (childrenCount - 1) * spacing);
	containerSize -= spacingSize;

	//If we found no children, exit now
	if (childrenCount == 0)
		return;

	//Now, using our parameters, work out what we have to work with
	int ungrowableHeight = 0;
	int growableHeight = 0;
	for (int i = 0; i < childrenCount; i++) {
		if (childrenAllowGrowth[i])
			growableHeight += childrenSizes[i];
		else
			ungrowableHeight += childrenSizes[i];
	}

	//Determine how the heights we can change compare to our remaining height
	int remainingHeight = containerSize - ungrowableHeight;
	
	//Mutate
	if (remainingHeight > 0 && growableHeight > 0) {
		//Calculate scaling factor
		float scaleFactor = remainingHeight / (float)growableHeight;

		//Assuming we have a positive amount of remaining height, scale all of our growable children by this factor
		for (int i = 0; i < childrenCount; i++) {
			//Make sure it's scalable
			if (!childrenAllowGrowth[i])
				continue;

			//Get the new height
			int newSize = (int)(childrenSizes[i] * scaleFactor);

			//Update state
			childrenSizes[i] = newSize;
		}
	}
	else {
		//Set all to zero height, as we don't have enough space at all (or we have none)
		for (int i = 0; i < childrenCount; i++) {
			//Make sure it's scalable
			if (!childrenAllowGrowth[i])
				continue;

			//Update state
			childrenSizes[i] = 0;
		}
	}

	//Now that we've got all of the sizes worked out, apply them to the children
	cursor = 0;
	int offset = 0;
	childrenCount = 0;
	while (enumerate_children(&cursor)) {
		//Get object size
		int objectSize = childrenSizes[childrenCount++];

		//Get the offset of this object, making sure it is within bounds
		int objectOffset;
		if (offset + objectSize < containerSize)
			objectOffset = offset;
		else
			objectOffset = containerSize - objectSize;
		offset += objectSize;
		offset += spacing;

		//Apply
		stacked_apply(cursor, objectSize, objectOffset);
	}
}

void RaptorStackedDrawable::draw_self() {
	//nothing special
}

void RaptorStackedDrawable::tick_self() {
	//nothing special
}

void RaptorStackedDrawable::set_padding(int padding) {
	set_padding(padding, padding);
}

void RaptorStackedDrawable::set_padding(int paddingHoriz, int paddingVert) {
	set_padding(paddingHoriz, paddingVert, paddingHoriz, paddingVert);
}

void RaptorStackedDrawable::set_padding(int paddingLeft, int paddingTop, int paddingRight, int paddingBottom) {
	this->padding_left = paddingLeft;
	this->padding_top = paddingTop;
	this->padding_right = paddingRight;
	this->padding_bottom = paddingBottom;
	invalidate_layout();
}

void RaptorStackedDrawable::set_spacing(int spacing) {
	this->spacing = spacing;
	invalidate_layout();
}