// license:BSD-3-Clause
// copyright-holders:Dirk Best
/***************************************************************************

    PK-8020

    Disk image format

***************************************************************************/
#ifndef MAME_FORMATS_PK8020_DSK_H
#define MAME_FORMATS_PK8020_DSK_H

#pragma once

#include "wd177x_dsk.h"

class pk8020_format : public wd177x_format
{
public:
	pk8020_format();

	virtual const char *name() const noexcept override;
	virtual const char *description() const noexcept override;
	virtual const char *extensions() const noexcept override;

private:
	static const format formats[];
};

extern const pk8020_format FLOPPY_PK8020_FORMAT;

#endif // MAME_FORMATS_PK8020_DSK_H
