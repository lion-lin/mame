// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    Coleco Adam Serial/Parallel Interface emulation

**********************************************************************/

#include "emu.h"
#include "spi.h"



//**************************************************************************
//  DEVICE DEFINITIONS
//**************************************************************************

DEFINE_DEVICE_TYPE(ADAM_SPI, adam_spi_device, "adam_spi", "Adam SPI")


//-------------------------------------------------
//  ROM( adam_spi )
//-------------------------------------------------

ROM_START( adam_spi )
	ROM_REGION( 0x800, "m6801", 0 )
	ROM_LOAD( "spi.bin", 0x000, 0x800, CRC(4ba30352) SHA1(99fe5aebd505a208bea6beec5d7322b15426e9c1) )
ROM_END


//-------------------------------------------------
//  rom_region - device-specific ROM region
//-------------------------------------------------

const tiny_rom_entry *adam_spi_device::device_rom_region() const
{
	return ROM_NAME( adam_spi );
}


//-------------------------------------------------
//  ADDRESS_MAP( adam_spi_mem )
//-------------------------------------------------

void adam_spi_device::adam_spi_mem(address_map &map)
{
	map(0x0100, 0x0103).rw("epci", FUNC(scn2661a_device::read), FUNC(scn2661a_device::write));
	map(0x0104, 0x01ff).ram();
}


//-------------------------------------------------
//  device_add_mconfig - add device configuration
//-------------------------------------------------

void adam_spi_device::device_add_mconfig(machine_config &config)
{
	M6801(config, m_maincpu, XTAL(4'000'000));
	m_maincpu->set_addrmap(AS_PROGRAM, &adam_spi_device::adam_spi_mem);
	m_maincpu->in_p2_cb().set(FUNC(adam_spi_device::p2_r));
	m_maincpu->out_p2_cb().set(FUNC(adam_spi_device::p2_w));

	scn2661a_device &epci(SCN2661A(config, "epci", XTAL(4'915'200)));
	epci.txd_handler().set("rs232", FUNC(rs232_port_device::write_txd));
	epci.rts_handler().set("rs232", FUNC(rs232_port_device::write_rts));
	epci.dtr_handler().set("rs232", FUNC(rs232_port_device::write_dtr));

	rs232_port_device &rs232(RS232_PORT(config, "rs232", default_rs232_devices, nullptr));
	rs232.rxd_handler().set("epci", FUNC(scn2661a_device::rxd_w));
	rs232.cts_handler().set("epci", FUNC(scn2661a_device::cts_w));
	rs232.dsr_handler().set("epci", FUNC(scn2661a_device::dsr_w));
	rs232.dcd_handler().set("epci", FUNC(scn2661a_device::dcd_w));

	centronics_device &centronics(CENTRONICS(config, "centronics", centronics_devices, "printer"));
	centronics.set_data_input_buffer("cent_data_in");
	INPUT_BUFFER(config, "cent_data_in");
	output_latch_device &cent_data_out(OUTPUT_LATCH(config, "cent_data_out"));
	centronics.set_output_latch(cent_data_out);
}



//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  adam_spi_device - constructor
//-------------------------------------------------

adam_spi_device::adam_spi_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, ADAM_SPI, tag, owner, clock)
	, device_adamnet_card_interface(mconfig, *this)
	, m_maincpu(*this, "m6801")
{
}


//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void adam_spi_device::device_start()
{
}


//-------------------------------------------------
//  adamnet_reset_w -
//-------------------------------------------------

void adam_spi_device::adamnet_reset_w(int state)
{
	m_maincpu->set_input_line(INPUT_LINE_RESET, state);
}


//-------------------------------------------------
//  p2_r -
//-------------------------------------------------

uint8_t adam_spi_device::p2_r()
{
	/*

	    bit     description

	    0       mode bit 0
	    1       mode bit 1
	    2       mode bit 2
	    3       NET RXD
	    4

	*/

	uint8_t data = M6801_MODE_7;

	// NET RXD
	data |= m_bus->rxd_r(this) << 3;

	return data;
}


//-------------------------------------------------
//  p2_w -
//-------------------------------------------------

void adam_spi_device::p2_w(uint8_t data)
{
	/*

	    bit     description

	    0
	    1
	    2
	    3
	    4       NET TXD

	*/

	m_bus->txd_w(this, BIT(data, 4));
}
