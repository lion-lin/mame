// license:BSD-3-Clause
// copyright-holders:Curt Coder
/*********************************************************************

    formats/tandy2k_dsk.h

    Tandy 2000 disk format

*********************************************************************/
#ifndef MAME_FORMATS_TANDY2K_DSK_H
#define MAME_FORMATS_TANDY2K_DSK_H

#pragma once

#include "upd765_dsk.h"

class tandy2k_format : public upd765_format
{
public:
	tandy2k_format();

	virtual const char *name() const noexcept override;
	virtual const char *description() const noexcept override;
	virtual const char *extensions() const noexcept override;

private:
	static const format formats[];
};

extern const tandy2k_format FLOPPY_TANDY_2000_FORMAT;

#endif // MAME_FORMATS_TANDY2K_DSK_H
