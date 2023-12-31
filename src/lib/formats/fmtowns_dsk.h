// license:BSD-3-Clause
// copyright-holders:Barry Rodewald
/*
 * fmtowns_dsk.h
 *
 *  Created on: 23/03/2014
 */
#ifndef MAME_FORMATS_FMTOWNS_DSK_H
#define MAME_FORMATS_FMTOWNS_DSK_H

#pragma once

#include "wd177x_dsk.h"

class fmtowns_format : public wd177x_format
{
public:
	fmtowns_format();

	virtual const char *name() const noexcept override;
	virtual const char *description() const noexcept override;
	virtual const char *extensions() const noexcept override;

private:
	static const format formats[];
};

extern const fmtowns_format FLOPPY_FMTOWNS_FORMAT;

#endif // MAME_FORMATS_FMTOWNS_DSK_H
