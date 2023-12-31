// license:BSD-3-Clause
// copyright-holders:Curt Coder
/*********************************************************************

    formats/ep64_dsk.cpp

    Enterprise Sixty Four disk image format

*********************************************************************/

#include "formats/ep64_dsk.h"

ep64_format::ep64_format() : wd177x_format(formats)
{
}

const char *ep64_format::name() const noexcept
{
	return "ep64";
}

const char *ep64_format::description() const noexcept
{
	return "Enteprise Sixty Four disk image";
}

const char *ep64_format::extensions() const noexcept
{
	return "img";
}

// Unverified gap sizes
const ep64_format::format ep64_format::formats[] = {
	{   /*  720K 3 1/2 inch double density */
		floppy_image::FF_35,  floppy_image::DSDD, floppy_image::MFM,
		2000,  9, 80, 2, 512, {}, 1, {}, 100, 22, 84
	},
	{}
};

const ep64_format FLOPPY_EP64_FORMAT;
