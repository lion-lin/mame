// license:BSD-3-Clause
// copyright-holders:Barry Rodewald
/*
 * magicsound.cpp
 *
 *  Magic Sound Board for the Aleste 520EX
 *
 */

#include "emu.h"
#include "magicsound.h"
#include "speaker.h"


//**************************************************************************
//  DEVICE DEFINITIONS
//**************************************************************************

DEFINE_DEVICE_TYPE(AL_MAGICSOUND, al_magicsound_device, "al_magicsound", "Aleste Magic Sound Board")


void al_magicsound_device::device_add_mconfig(machine_config &config)
{
	AM9517A(config, m_dmac, DERIVED_CLOCK(1, 1));  // CLK from expansion port
	// According to the schematics, the TC pin (EOP on western chips) is connected to NMI on the expansion port.
	// NMIs seem to occur too quickly when this is active, so either EOP is not triggered at the correct time, or
	// the K1810WT37 is different to the i8237/AM9517A
	//m_dmac->out_eop_callback().set(DEVICE_SELF_OWNER, FUNC(cpc_expansion_slot_device::nmi_w))/*.invert()*/;
	m_dmac->out_hreq_callback().set(m_dmac, FUNC(am9517a_device::hack_w));
	m_dmac->in_memr_callback().set(FUNC(al_magicsound_device::dma_read_byte));
	m_dmac->out_iow_callback<0>().set(FUNC(al_magicsound_device::dma_write_byte));
	m_dmac->out_iow_callback<1>().set(FUNC(al_magicsound_device::dma_write_byte));
	m_dmac->out_iow_callback<2>().set(FUNC(al_magicsound_device::dma_write_byte));
	m_dmac->out_iow_callback<3>().set(FUNC(al_magicsound_device::dma_write_byte));
	m_dmac->out_dack_callback<0>().set(FUNC(al_magicsound_device::dack0_w));
	m_dmac->out_dack_callback<1>().set(FUNC(al_magicsound_device::dack1_w));
	m_dmac->out_dack_callback<2>().set(FUNC(al_magicsound_device::dack2_w));
	m_dmac->out_dack_callback<3>().set(FUNC(al_magicsound_device::dack3_w));

	// Timing does not seem to be correct.
	// According to the schematics, the clock is from the clock pin on the expansion port (4MHz), and
	// passes through an inverter to each CLK pin on both timers.  This seems to be too fast.
	// Timer outputs to SAM0/1/2/3 are sample clocks for each sound channel, D/A0 is the low bit of the channel select.
	PIT8254(config, m_timer1, 0);
	m_timer1->set_clk<0>(4000000);
	m_timer1->out_handler<0>().set(FUNC(al_magicsound_device::sam0_w));
	m_timer1->set_clk<1>(4000000);
	m_timer1->out_handler<1>().set(FUNC(al_magicsound_device::sam1_w));
	m_timer1->set_clk<2>(4000000);
	m_timer1->out_handler<2>().set(FUNC(al_magicsound_device::sam2_w));

	PIT8254(config, m_timer2, 0);
	m_timer2->set_clk<0>(4000000);
	m_timer2->out_handler<0>().set(FUNC(al_magicsound_device::sam3_w));
	m_timer2->set_clk<1>(4000000);
	m_timer2->out_handler<1>().set(FUNC(al_magicsound_device::da0_w));
	m_timer2->set_clk<2>(4000000);

	SPEAKER(config, "speaker").front_center();
	DAC_8BIT_R2R(config, m_dac, 0).add_route(ALL_OUTPUTS, "speaker", 0.5); // unknown DAC
	// no pass-through(?)
}


//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

al_magicsound_device::al_magicsound_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	device_t(mconfig, AL_MAGICSOUND, tag, owner, clock),
	device_cpc_expansion_card_interface(mconfig, *this), m_slot(nullptr),
	m_dac(*this,"dac"),
	m_dmac(*this,"dmac"),
	m_timer1(*this,"timer1"),
	m_timer2(*this,"timer2"),
	m_current_channel(0), m_ramptr(nullptr), m_current_output(0)
{
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void al_magicsound_device::device_start()
{
	m_slot = dynamic_cast<cpc_expansion_slot_device *>(owner());
	address_space &space = m_slot->cpu().space(AS_IO);
	space.install_readwrite_handler(0xf8d0,0xf8df, read8sm_delegate(*this, FUNC(al_magicsound_device::dmac_r)), write8sm_delegate(*this, FUNC(al_magicsound_device::dmac_w)));
	space.install_write_handler(0xf9d0,0xf9df, write8sm_delegate(*this, FUNC(al_magicsound_device::timer_w)));
	space.install_write_handler(0xfad0,0xfadf, write8sm_delegate(*this, FUNC(al_magicsound_device::volume_w)));
	space.install_write_handler(0xfbd0,0xfbdf, write8sm_delegate(*this, FUNC(al_magicsound_device::mapper_w)));

	m_ramptr = machine().device<ram_device>(":" RAM_TAG);

	save_item(NAME(m_output));
}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void al_magicsound_device::device_reset()
{
	m_current_channel = -1;
	m_current_output = 0;
	set_timer_gate(false);
}

uint8_t al_magicsound_device::dmac_r(offs_t offset)
{
	return m_dmac->read(offset);
}

void al_magicsound_device::dmac_w(offs_t offset, uint8_t data)
{
	m_dmac->write(offset,data);
}

void al_magicsound_device::timer_w(offs_t offset, uint8_t data)
{
	// can both PITs be selected at the same time?
	if(offset & 0x08)
		m_timer1->write(offset & 0x03,data);
	if(offset & 0x04)
		m_timer2->write(offset & 0x03,data);
}

void al_magicsound_device::volume_w(offs_t offset, uint8_t data)
{
	m_volume[offset & 0x03] = data & 0x3f;
}

void al_magicsound_device::mapper_w(offs_t offset, uint8_t data)
{
	uint8_t channel = (offset & 0x0c) >> 2;
	uint8_t page = offset & 0x03;
	m_page[channel][page] = (~(data) & 0x3f) * 0x4000;
	set_timer_gate(true);
}

void al_magicsound_device::da0_w(int state)
{
	m_dac->write(m_output[m_current_output++]);
	if(m_current_output > 3)
		m_current_output = 0;
}

void al_magicsound_device::dack0_w(int state) { m_dack[0] = state; }
void al_magicsound_device::dack1_w(int state) { m_dack[1] = state; }
void al_magicsound_device::dack2_w(int state) { m_dack[2] = state; }
void al_magicsound_device::dack3_w(int state) { m_dack[3] = state; }

void al_magicsound_device::sam0_w(int state) { m_current_channel = 0; if(m_dack[0] && state) m_dmac->dreq0_w(1); }
void al_magicsound_device::sam1_w(int state) { m_current_channel = 1; if(m_dack[1] && state) m_dmac->dreq1_w(1); }
void al_magicsound_device::sam2_w(int state) { m_current_channel = 2; if(m_dack[2] && state) m_dmac->dreq2_w(1); }
void al_magicsound_device::sam3_w(int state) { m_current_channel = 3; if(m_dack[3] && state) m_dmac->dreq3_w(1); }

uint8_t al_magicsound_device::dma_read_byte(offs_t offset)
{
	uint8_t ret = 0xff;
	uint8_t page = (offset & 0xc000) >> 14;

	if(m_current_channel != -1)
		ret = m_ramptr->read(m_page[m_current_channel][page] + (offset & 0x3fff));
	return ret;
}

void al_magicsound_device::dma_write_byte(uint8_t data)
{
	m_output[m_current_channel] = data;
}

void al_magicsound_device::set_timer_gate(bool state)
{
	m_timer1->write_gate0(state);
	m_timer1->write_gate1(state);
	m_timer1->write_gate2(state);
	m_timer2->write_gate0(state);
	m_timer2->write_gate1(state);
	m_timer2->write_gate2(state);
}
