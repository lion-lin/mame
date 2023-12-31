// license:BSD-3-Clause
// copyright-holders:Carl
#ifndef MAME_FORMATS_ATOM_DSK_H
#define MAME_FORMATS_ATOM_DSK_H

#pragma once

#include "flopimg.h"
#include "wd177x_dsk.h"

class atom_format : public wd177x_format
{
public:
	atom_format();

	virtual const char *name() const noexcept override;
	virtual const char *description() const noexcept override;
	virtual const char *extensions() const noexcept override;

private:
	static const format formats[];
};

extern const atom_format FLOPPY_ATOM_FORMAT;

#endif // MAME_FORMATS_ATOM_DSK_H
