// license:BSD-3-Clause
// copyright-holders:Dirk Best
/***************************************************************************

    JPM Give us a Break

    Disk image format

***************************************************************************/

#include "guab_dsk.h"

guab_format::guab_format() : wd177x_format(formats)
{
}

const char *guab_format::name() const noexcept
{
	return "guab";
}

const char *guab_format::description() const noexcept
{
	return "JPM Give us a Break disk image";
}

const char *guab_format::extensions() const noexcept
{
	return "dsk";
}

// gap sizes unverified
const guab_format::format guab_format::formats[] =
{
	{
		floppy_image::FF_35, floppy_image::DSDD, floppy_image::MFM,
		2000, 18, 80, 2, 256, {}, 0, {}, 80, 22, 24
	},
	{}
};

const guab_format FLOPPY_GUAB_FORMAT;
