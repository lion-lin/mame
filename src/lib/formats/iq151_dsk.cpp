// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
/*********************************************************************

    formats/iq151_dsk.cpp

    iq151 format

*********************************************************************/

#include "formats/iq151_dsk.h"

iq151_format::iq151_format() : upd765_format(formats)
{
}

const char *iq151_format::name() const noexcept
{
	return "iq151";
}

const char *iq151_format::description() const noexcept
{
	return "IQ151 disk image";
}

const char *iq151_format::extensions() const noexcept
{
	return "iqd";
}

// Unverified gap sizes.  May be FM.
const iq151_format::format iq151_format::formats[] = {
	{
		floppy_image::FF_8, floppy_image::SSSD, floppy_image::MFM,
		2000, // maybe
		26, 77, 1,
		128, {},
		1, {},
		80, 50, 22, 80
	},
	{}
};

const iq151_format FLOPPY_IQ151_FORMAT;
