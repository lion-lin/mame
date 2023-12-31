// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
/*********************************************************************

    formats/itt3030_dsk.cpp

    ITT3030 560K disk image format



*********************************************************************/

#include "formats/itt3030_dsk.h"

itt3030_format::itt3030_format() : wd177x_format(formats)
{
}

const char *itt3030_format::name() const noexcept
{
	return "itt3030";
}

const char *itt3030_format::description() const noexcept
{
	return "ITT3030 disk image";
}

const char *itt3030_format::extensions() const noexcept
{
	return "dsk";
}

// gap info is a total guess
const itt3030_format::format itt3030_format::formats[] = {
	{   /*  5,25" DS DD 70 tracks 16 SPT 256 bytes/sector */
		floppy_image::FF_525,  floppy_image::DSQD, floppy_image::MFM,
		2000,
		16, 70, 2,
		256, {},
		1, {},
		32, 22, 31
	},
	{/*  5,25" DS DD 35 tracks 16 SPT 256 bytes/sector */
		floppy_image::FF_525,  floppy_image::DSDD, floppy_image::MFM,
		2000,
		16, 35, 2,
		256, {},
		1, {},
		32, 22, 31
	},
	{}
};



const itt3030_format FLOPPY_ITT3030_FORMAT;
