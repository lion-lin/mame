// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
/*********************************************************************

    formats/itt3030_dsk.h

    ITT3030 disk image format

*********************************************************************/
#ifndef MAME_FORMATS_ITT3030_DSK_H
#define MAME_FORMATS_ITT3030_DSK_H

#pragma once

#include "wd177x_dsk.h"

class itt3030_format : public wd177x_format
{
public:
	itt3030_format();

	virtual const char *name() const noexcept override;
	virtual const char *description() const noexcept override;
	virtual const char *extensions() const noexcept override;

private:
	static const format formats[];
};


extern const itt3030_format FLOPPY_ITT3030_FORMAT;

#endif // MAME_FORMATS_ITT3030_DSK_H
