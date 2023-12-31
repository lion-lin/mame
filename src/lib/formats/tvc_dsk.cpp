// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
/*********************************************************************

    formats/tvc_dsk.cpp

    tvc format

*********************************************************************/

#include "formats/tvc_dsk.h"

tvc_format::tvc_format() : wd177x_format(formats)
{
}

const char *tvc_format::name() const noexcept
{
	return "tvc";
}

const char *tvc_format::description() const noexcept
{
	return "Videoton TVC HBF disk image";
}

const char *tvc_format::extensions() const noexcept
{
	return "img,dsk";
}

// Unverified gap sizes
const tvc_format::format tvc_format::formats[] =
{
	{   //  720K 5.25 inch
		floppy_image::FF_525,  floppy_image::DSQD, floppy_image::MFM,
		2000, 9, 80, 2, 512, {}, 1, {}, 100, 22, 20
	},
	{   //  360K 5.25 inch
		floppy_image::FF_525,  floppy_image::SSQD, floppy_image::MFM,
		2000, 9, 80, 1, 512, {}, 1, {}, 100, 22, 20
	},
	{}
};

const tvc_format FLOPPY_TVC_FORMAT;
