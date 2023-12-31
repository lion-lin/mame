// license:BSD-3-Clause
// copyright-holders:Sven Schnelle

#ifndef MAME_BUS_HPDIO_98265A_H
#define MAME_BUS_HPDIO_98265A_H

#pragma once

#include "hp_dio.h"
#include "machine/mb87030.h"
#include "bus/scsi/scsi.h"
#include "bus/scsi/scsicd.h"

namespace bus::hp_dio {

class dio16_98265a_device :
		public device_t,
		public device_dio32_card_interface
{
public:
	// construction/destruction
	dio16_98265a_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	void mb87030(device_t *device);

protected:
	dio16_98265a_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock);


	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	virtual ioport_constructor device_input_ports() const override;
	virtual void device_add_mconfig(machine_config &config) override;

	uint16_t io_r(offs_t offset);
	void io_w(offs_t offset, uint16_t data);

	void dmack_w_in(int channel, uint8_t data) override;
	uint8_t dmack_r_in(int channel) override;

	void dmar0_w(int state);

	void irq_w(int state);

	required_device<nscsi_bus_device> m_scsibus;
	required_device<mb87030_device> m_spc;
private:

	static constexpr int REG_CONTROL_DE0 = (1 << 0);
	static constexpr int REG_CONTROL_DE1 = (1 << 1);

	static void mb87030_scsi_adapter(device_t *device);
	required_ioport m_sw1;
	required_ioport m_sw2;
	int get_int_level();
	void update_irq(bool state);
	void update_dma();
	bool     m_installed_io;
	uint8_t  m_control;

	bool m_irq_state;
	bool m_dmar0;
};

} // namespace bus::hp_dio

DECLARE_DEVICE_TYPE_NS(HPDIO_98265A, bus::hp_dio, dio16_98265a_device)

#endif // MAME_BUS_HPDIO_98265A_H
