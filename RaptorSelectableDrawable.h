#pragma once

#include "RaptorDrawable.h"

class RaptorSelectableDrawable : public RaptorDrawable {

	virtual void describe(int* flags) override;

public:
	virtual void hover_entered() = 0;
	virtual void hover_left() = 0;

};