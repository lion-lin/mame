// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
/*********************************************************************

    formats/smx_dsk.h

    Specialist MX disk images

*********************************************************************/
#ifndef MAME_FORMATS_SMX_DSK_H
#define MAME_FORMATS_SMX_DSK_H

#pragma once

#include "wd177x_dsk.h"

class smx_format : public wd177x_format
{
public:
	smx_format();

	virtual const char *name() const noexcept override;
	virtual const char *description() const noexcept override;
	virtual const char *extensions() const noexcept override;

private:
	static const format formats[];
};

extern const smx_format FLOPPY_SMX_FORMAT;

#endif // MAME_FORMATS_SMX_DSK_H
