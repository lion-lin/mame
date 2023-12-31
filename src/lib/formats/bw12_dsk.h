// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
/*********************************************************************

    formats/bw12_dsk.h

    Bonwell 12/14 format

*********************************************************************/
#ifndef MAME_FORMATS_BW12_DSK_H
#define MAME_FORMATS_BW12_DSK_H

#pragma once

#include "upd765_dsk.h"

class bw12_format : public upd765_format
{
public:
	bw12_format();

	virtual const char *name() const noexcept override;
	virtual const char *description() const noexcept override;
	virtual const char *extensions() const noexcept override;

private:
	static const format formats[];
};

extern const bw12_format FLOPPY_BW12_FORMAT;

#endif // MAME_FORMATS_BW12_DSK_H
