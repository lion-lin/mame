// license:BSD-3-Clause
// copyright-holders:Dirk Best
/***************************************************************************

    Hector Minidisc

    Disk image format

***************************************************************************/
#ifndef MAME_FORMATS_HECTOR_MINIDISC_H
#define MAME_FORMATS_HECTOR_MINIDISC_H

#pragma once

#include "upd765_dsk.h"

class hmd_format : public upd765_format
{
public:
	hmd_format();

	virtual const char *name() const noexcept override;
	virtual const char *description() const noexcept override;
	virtual const char *extensions() const noexcept override;

private:
	static const format formats[];
};

extern const hmd_format FLOPPY_HMD_FORMAT;

#endif // MAME_FORMATS_HECTOR_MINIDISC_H
