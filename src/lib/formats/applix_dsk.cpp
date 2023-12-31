// license:BSD-3-Clause
// copyright-holders:Robbbert
/*********************************************************************

    formats/applix_dsk.cpp

    Applix disk image format

*********************************************************************/

#include "formats/applix_dsk.h"

applix_format::applix_format() : wd177x_format(formats)
{
}

const char *applix_format::name() const noexcept
{
	return "applix";
}

const char *applix_format::description() const noexcept
{
	return "Applix disk image";
}

const char *applix_format::extensions() const noexcept
{
	return "raw";
}

// Unverified gap sizes
const applix_format::format applix_format::formats[] = {
	{   /*  800K 3 1/2 inch double density */
		floppy_image::FF_35,  floppy_image::DSDD, floppy_image::MFM,
		2000,  5, 80, 2, 1024, {}, 1, {}, 100, 22, 84
	},
	{}
};

const applix_format FLOPPY_APPLIX_FORMAT;
