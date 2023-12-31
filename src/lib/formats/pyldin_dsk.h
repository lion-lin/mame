// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
/*********************************************************************

    formats/pyldin_dsk.h

    pyldin format

*********************************************************************/
#ifndef MAME_FORMATS_PYLDIN_DSK_H
#define MAME_FORMATS_PYLDIN_DSK_H

#pragma once

#include "upd765_dsk.h"

class pyldin_format : public upd765_format
{
public:
	pyldin_format();

	virtual const char *name() const noexcept override;
	virtual const char *description() const noexcept override;
	virtual const char *extensions() const noexcept override;

private:
	static const format formats[];
};

extern const pyldin_format FLOPPY_PYLDIN_FORMAT;

#endif // MAME_FORMATS_PYLDIN_DSK_H
