#include "RaptorSelectableDrawable.h"

void RaptorSelectableDrawable::describe(int* flags)
{
	//Call base
	RaptorDrawable::describe(flags);

	//Set
	*flags |= RAPTOR_DRAWABLE_FLAG_SELECTABLE;
}