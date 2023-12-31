// license:BSD-3-Clause
// copyright-holders:Olivier Galibert, Robbbert
/*********************************************************************

    formats/kaypro_dsk.h

    Kaypro disk image format

*********************************************************************/
#ifndef MAME_FORMATS_KAYPRO_DSK_H
#define MAME_FORMATS_KAYPRO_DSK_H

#pragma once

#include "upd765_dsk.h"

class kayproii_format : public upd765_format {
public:
	kayproii_format();

	virtual const char *name() const noexcept override;
	virtual const char *description() const noexcept override;
	virtual const char *extensions() const noexcept override;

private:
	static const format formats[];
};

class kaypro2x_format : public upd765_format {
public:
	kaypro2x_format();

	virtual const char *name() const noexcept override;
	virtual const char *description() const noexcept override;
	virtual const char *extensions() const noexcept override;
private:
	static const format formats[];
};

extern const kayproii_format FLOPPY_KAYPROII_FORMAT;
extern const kaypro2x_format FLOPPY_KAYPRO2X_FORMAT;

#endif // MAME_FORMATS_KAYPRO_DSK_H
