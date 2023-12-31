// license:BSD-3-Clause
// copyright-holders:Curt Coder
/*********************************************************************

    formats/abcfd2_dsk.cpp

    Scandia Metric ABC FD2 disk image formats

*********************************************************************/

#include "formats/abcfd2_dsk.h"

abc_fd2_format::abc_fd2_format() : wd177x_format(formats)
{
}

const char *abc_fd2_format::name() const noexcept
{
	return "abc_fd2";
}

const char *abc_fd2_format::description() const noexcept
{
	return "Scandia Metric ABC FD2 disk image";
}

const char *abc_fd2_format::extensions() const noexcept
{
	return "dsk";
}

const abc_fd2_format::format abc_fd2_format::formats[] = {
	// track description
	// 28xff 6x00 fe 2x00 01 00 f7 11xff 6x00 fb 128xe5 f7
	// 27xff 6x00 fe 2x00 02 00 f7 11xff 6x00 fb 128xe5 f7
	// 27xff 6x00 fe 2x00 03 00 f7 11xff 6x00 fb 128xe5 f7
	// 27xff 6x00 fe 2x00 04 00 f7 11xff 6x00 fb 128xe5 f7
	// 27xff 6x00 fe 2x00 05 00 f7 11xff 6x00 fb 128xe5 f7
	// 27xff 6x00 fe 2x00 06 00 f7 11xff 6x00 fb 128xe5 f7
	// 27xff 6x00 fe 2x00 07 00 f7 11xff 6x00 fb 128xe5 f7
	// 27xff 6x00 fe 2x00 08 00 f7 11xff 6x00 fb 128xe5 f7
	// 27xff 6x00 fe 2x00 09 00 f7 11xff 6x00 fb 128xe5 f7
	// 27xff 6x00 fe 2x00 0a 00 f7 11xff 6x00 fb 128xe5 f7
	// 27xff 6x00 fe 2x00 0b 00 f7 11xff 6x00 fb 128xe5 f7
	// 27xff 6x00 fe 2x00 0c 00 f7 11xff 6x00 fb 128xe5 f7
	// 27xff 6x00 fe 2x00 0d 00 f7 11xff 6x00 fb 128xe5 f7
	// 27xff 6x00 fe 2x00 0e 00 f7 11xff 6x00 fb 128xe5 f7
	// 27xff 6x00 fe 2x00 0f 00 f7 11xff 6x00 fb 128xe5 f7
	// 27xff 6x00 fe 2x00 10 00 f7 11xff 6x00 fb 128xe5 f7
	// 117xff

	{   //  80K 5 1/4 inch single density single sided
		floppy_image::FF_525, floppy_image::SSSD, floppy_image::FM,
		4000, 16, 40, 1, 128, {}, 1, {}, 28, 11, 27
	},

	{}
};

const abc_fd2_format FLOPPY_ABC_FD2_FORMAT;
