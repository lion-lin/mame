// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    Intelligent Software EXDOS Disk Controller Module emulation

**********************************************************************/

/*

Floppy Drive Controller PCB Layout
----------------------------------

INTELLIGENT SOFTWARE LTD DISK CONTROLLER
ISS1
 |--------------------------------------------|
 |                                            |
 |                                            |
|-|  7438  74LS273             WD1770         |
|I|                                           |
|D|                                           |
|C|                                  EPROM.IC2|
|3|           74LS32  74LS02  74LS266         |
|4|  7438                                     |
|-|      74LS126  74LS10  74LS245  74LS266    |
 |                                            |
 |                                            |
 |----------------------------|||||||||||||||||
                              |---------------|
Notes: (All IC's shown)

This PCB plugs into the external expansion connector on the right side of the mainboard

      EPROM.IC2 - 16k x8-bit EPROM labelled 'EXDOS V1.0 P/N 08-60' (DIP28)
         WD1770 - Western Digital WD1770 Floppy Drive Controller (DIP28)
         74LS02 - Quad 2-Input NOR Gate (DIP14)
         74LS10 - Triple 3-input NAND Gate (DIP14)
         74LS32 - Quad 2-Input Positive OR Gate (DIP14)
           7438 - Quad 2-input NAND Buffer (DIP14)
        74LS126 - Quad Bus Buffer (DIP14)
        74LS245 - Octal Bus Tranceiver with Tri-State Outputs (DIP20)
        74LS266 - Quad EXCLUSIVE-NOR Gate (DIP14)
        74LS273 - Octal D-Type Flip-Flop With Clear (DIP20)
          IDC34 - IDC 34-way flat cable connector for floppy drive data cable

*/

#include "emu.h"
#include "exdos.h"

#include "formats/ep64_dsk.h"



//**************************************************************************
//  MACROS/CONSTANTS
//**************************************************************************

#define WD1770_TAG  "u1"



//**************************************************************************
//  DEVICE DEFINITIONS
//**************************************************************************

DEFINE_DEVICE_TYPE(EP64_EXDOS, ep64_exdos_device, "ep64_exdos", "EP64 EXDOS")


//-------------------------------------------------
//  ROM( ep64_exdos )
//-------------------------------------------------

ROM_START( ep64_exdos )
	ROM_REGION( 0x8000, "rom", 0 )
	ROM_LOAD( "exdos13.rom", 0x0000, 0x8000, CRC(d1d7e157) SHA1(31c8be089526aa8aa019c380cdf51ddd3ee76454) )
ROM_END


//-------------------------------------------------
//  rom_region - device-specific ROM region
//-------------------------------------------------

const tiny_rom_entry *ep64_exdos_device::device_rom_region() const
{
	return ROM_NAME( ep64_exdos );
}


//-------------------------------------------------
//  SLOT_INTERFACE( ep64_exdos_floppies )
//-------------------------------------------------

void ep64_exdos_device::floppy_formats(format_registration &fr)
{
	fr.add_mfm_containers();
	fr.add(FLOPPY_EP64_FORMAT);
}

static void ep64_exdos_floppies(device_slot_interface &device)
{
	device.option_add("35dd", FLOPPY_35_DD);
}


//-------------------------------------------------
//  device_add_mconfig - add device configuration
//-------------------------------------------------

void ep64_exdos_device::device_add_mconfig(machine_config &config)
{
	WD1770(config, m_fdc, 8_MHz_XTAL);

	for (auto &floppy : m_floppy)
		FLOPPY_CONNECTOR(config, floppy, ep64_exdos_floppies, nullptr, ep64_exdos_device::floppy_formats);
	m_floppy[0]->set_default_option("35dd");
}


//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  ep64_exdos_device - constructor
//-------------------------------------------------

ep64_exdos_device::ep64_exdos_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	device_t(mconfig, EP64_EXDOS, tag, owner, clock),
	device_ep64_expansion_bus_card_interface(mconfig, *this),
	m_fdc(*this, WD1770_TAG),
	m_floppy(*this, WD1770_TAG":%u", 0U),
	m_selected_floppy(nullptr),
	m_rom(*this, "rom")
{
}


//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void ep64_exdos_device::device_start()
{
	m_slot->program().install_rom(0x080000, 0x087fff, m_rom->base());

	m_slot->io().install_readwrite_handler(0x10, 0x13, 0, 0x04, 0, read8sm_delegate(*m_fdc, FUNC(wd_fdc_device_base::read)), write8sm_delegate(*m_fdc, FUNC(wd_fdc_device_base::write)));
	m_slot->io().install_readwrite_handler(0x18, 0x18, 0, 0x04, 0, read8smo_delegate(*this, FUNC(ep64_exdos_device::read)), write8smo_delegate(*this, FUNC(ep64_exdos_device::write)));
}


//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void ep64_exdos_device::device_reset()
{
	m_fdc->reset();

	m_selected_floppy = nullptr;
	m_fdc->set_floppy(m_selected_floppy);
	m_fdc->dden_w(0);
}


//-------------------------------------------------
//  read -
//-------------------------------------------------

uint8_t ep64_exdos_device::read()
{
	/*

	    bit     description

	    0
	    1       INTRQ
	    2
	    3
	    4
	    5
	    6       DCHG
	    7       DRQ

	*/

	uint8_t data = 0;

	data |= m_fdc->intrq_r() << 1;
	data |= m_fdc->drq_r() << 7;

	data |= (m_selected_floppy ? m_selected_floppy->dskchg_r() : 1) << 6;

	return data;
}


//-------------------------------------------------
//  write -
//-------------------------------------------------

void ep64_exdos_device::write(uint8_t data)
{
	/*

	    bit     description

	    0       SELECT 0
	    1       SELECT 1
	    2       SELECT 2
	    3       SELECT 3
	    4       SIDE 1
	    5       _DDEN
	    6       DISK CHANGE RESET
	    7       IN USE

	*/

	m_selected_floppy = nullptr;

	for (int n = 0; n < 4; n++)
	{
		if (BIT(data, n))
			m_selected_floppy = m_floppy[n]->get_device();
	}

	m_fdc->set_floppy(m_selected_floppy);

	if (m_selected_floppy)
		m_selected_floppy->ss_w(BIT(data, 4));

	m_fdc->dden_w(BIT(data, 5));
}
