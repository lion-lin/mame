// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
/*********************************************************************

    formats/iq151_dsk.h

    iq151 format

*********************************************************************/
#ifndef MAME_FORMATS_IQ151_DSK_H
#define MAME_FORMATS_IQ151_DSK_H

#pragma once

#include "upd765_dsk.h"

class iq151_format : public upd765_format
{
public:
	iq151_format();

	virtual const char *name() const noexcept override;
	virtual const char *description() const noexcept override;
	virtual const char *extensions() const noexcept override;

private:
	static const format formats[];
};

extern const iq151_format FLOPPY_IQ151_FORMAT;

#endif // MAME_FORMATS_IQ151_DSK_H
