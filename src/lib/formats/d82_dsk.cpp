// license:BSD-3-Clause
// copyright-holders:Curt Coder
/*********************************************************************

    formats/d82_dsk.cpp

    Commodore 8250/SFD-1001 sector disk image format

*********************************************************************/

#include "formats/d82_dsk.h"

d82_format::d82_format() : d80_format(file_formats)
{
}

const char *d82_format::name() const noexcept
{
	return "d82";
}

const char *d82_format::description() const noexcept
{
	return "Commodore 8250/SFD-1001 disk image";
}

const char *d82_format::extensions() const noexcept
{
	return "d82";
}

const d82_format::format d82_format::file_formats[] = {
	{ // d82, dos 2.5, 77 tracks, 2 heads, head/stepper 100 tpi
		floppy_image::FF_525, floppy_image::DSQD, 2083, 77, 2, 256, 21, 19 // TODO verify gaps
	},
	{}
};

const d82_format FLOPPY_D82_FORMAT;
