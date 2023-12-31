// license:BSD-3-Clause
// copyright-holders:R. Belmont

#ifndef MAME_BUS_HPDIO_98544_H
#define MAME_BUS_HPDIO_98544_H

#pragma once

#include "hp_dio.h"
#include "video/topcat.h"
//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> dio16_98544_device

namespace bus::hp_dio {

class dio16_98544_device :
	public device_t,
	public device_dio16_card_interface,
	public device_memory_interface
{
public:
	// construction/destruction
	dio16_98544_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	uint16_t rom_r(offs_t offset);
	void rom_w(offs_t offset, uint16_t data);

	required_device<topcat_device> m_topcat;
	uint32_t screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
 protected:
	dio16_98544_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock);

	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	// optional information overrides
	virtual void device_add_mconfig(machine_config &config) override;
	virtual const tiny_rom_entry *device_rom_region() const override;
	virtual space_config_vector memory_space_config() const override;
private:

	void vblank_w(int state);
	void int_w(int state);

	static constexpr int m_v_pix = 768;
	static constexpr int m_h_pix = 1024;

	const address_space_config m_space_config;
	void map(address_map &map);
	required_region_ptr<uint8_t> m_rom;
	required_shared_ptr<uint8_t> m_vram;

	uint8_t m_intreg;
};

} // namespace bus::hp_dio

// device type definition
DECLARE_DEVICE_TYPE_NS(HPDIO_98544, bus::hp_dio, dio16_98544_device)

#endif // MAME_BUS_HPDIO_98544_H
