// license:BSD-3-Clause
// copyright-holders:Curt Coder
/*********************************************************************

    formats/d81_dsk.h

    Commodore 1581 disk image format

*********************************************************************/
#ifndef MAME_FORMATS_D81_DSK_H
#define MAME_FORMATS_D81_DSK_H

#pragma once

#include "wd177x_dsk.h"

class d81_format : public wd177x_format
{
public:
	d81_format();

	virtual const char *name() const noexcept override;
	virtual const char *description() const noexcept override;
	virtual const char *extensions() const noexcept override;

	virtual floppy_image_format_t::desc_e* get_desc_mfm(const format &f, int &current_size, int &end_gap_index) const override;

private:
	static const format formats[];
};

extern const d81_format FLOPPY_D81_FORMAT;

#endif // MAME_FORMATS_D81_DSK_H
