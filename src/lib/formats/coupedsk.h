// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
/*************************************************************************

    formats/coupedsk.h

    SAM Coupe disk image formats

**************************************************************************/
#ifndef MAME_FORMATS_COUPEDSK_H
#define MAME_FORMATS_COUPEDSK_H

#pragma once

#include "flopimg.h"

class mgt_format : public floppy_image_format_t
{
public:
	mgt_format();

	virtual int identify(util::random_read &io, uint32_t form_factor, const std::vector<uint32_t> &variants) const override;
	virtual bool load(util::random_read &io, uint32_t form_factor, const std::vector<uint32_t> &variants, floppy_image &image) const override;
	virtual bool save(util::random_read_write &io, const std::vector<uint32_t> &variants, const floppy_image &image) const override;

	virtual const char *name() const noexcept override;
	virtual const char *description() const noexcept override;
	virtual const char *extensions() const noexcept override;
	virtual bool supports_save() const noexcept override;

	static const floppy_image_format_t::desc_e desc_10[];
};

extern const mgt_format FLOPPY_MGT_FORMAT;

#endif // MAME_FORMATS_COUPEDSK_H
