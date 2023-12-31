// license:BSD-3-Clause
// copyright-holders:Dirk Best
/***************************************************************************

    2D

    Disk image format used by early Sharp computers

***************************************************************************/

#include "2d_dsk.h"

_2d_format::_2d_format() : wd177x_format(formats)
{
}

const char *_2d_format::name() const noexcept
{
	return "2d";
}

const char *_2d_format::description() const noexcept
{
	return "2D disk image";
}

const char *_2d_format::extensions() const noexcept
{
	return "2d";
}

const _2d_format::format _2d_format::formats[] =
{
	{
		floppy_image::FF_525, floppy_image::DSDD, floppy_image::MFM,
		2000, 16, 40, 2, 256, {}, 1, {}, 32, 22, 54
	},
	{}
};

const _2d_format FLOPPY_2D_FORMAT;
