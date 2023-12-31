// license:BSD-3-Clause
// copyright-holders:Mike Balfour
/***************************************************************************

    Atari Basketball hardware

***************************************************************************/

#include "emu.h"
#include "bsktball.h"

/***************************************************************************
    bsktball_nmion_w
***************************************************************************/

void bsktball_state::nmion_w(int state)
{
	m_nmi_on = state;
}

/***************************************************************************
    bsktball_interrupt
***************************************************************************/

TIMER_DEVICE_CALLBACK_MEMBER(bsktball_state::bsktball_scanline)
{
	// NMI every 32V
	if (m_nmi_on)
		m_maincpu->pulse_input_line(INPUT_LINE_NMI, attotime::zero);
}


/***************************************************************************
    bsktball_ld_w
***************************************************************************/

void bsktball_state::ld1_w(int state)
{
	m_ld1 = state;
}

void bsktball_state::ld2_w(int state)
{
	m_ld2 = state;
}


/***************************************************************************
    bsktball_in0_r
***************************************************************************/

uint8_t bsktball_state::bsktball_in0_r()
{
	int p1_horiz;
	int p1_vert;
	int p2_horiz;
	int p2_vert;
	int temp;

	p1_horiz = ioport("TRACK0_X")->read();
	p1_vert  = ioport("TRACK0_Y")->read();
	p2_horiz = ioport("TRACK1_X")->read();
	p2_vert  = ioport("TRACK1_Y")->read();

	/* Set direction bits */

	/* P1 H DIR */
	if (p1_horiz > m_last_p1_horiz)
	{
		if ((p1_horiz - m_last_p1_horiz) > 128)
			m_dir2 = 0x40;
		else
			m_dir2 = 0;
	}
	else if (p1_horiz < m_last_p1_horiz)
	{
		if ((m_last_p1_horiz - p1_horiz) > 128)
			m_dir2 = 0;
		else
			m_dir2 = 0x40;
	}

	/* P1 V DIR */
	if (p1_vert > m_last_p1_vert)
	{
		if ((p1_vert - m_last_p1_vert) > 128)
			m_dir3 = 0;
		else
			m_dir3 = 0x80;
	}
	else if (p1_vert < m_last_p1_vert)
	{
		if ((m_last_p1_vert - p1_vert) > 128)
			m_dir3 = 0x80;
		else
			m_dir3 = 0;
	}

	/* P2 H DIR */
	if (p2_horiz > m_last_p2_horiz)
	{
		if ((p2_horiz - m_last_p2_horiz) > 128)
			m_dir0 = 0x10;
		else
			m_dir0 = 0;
	}
	else if (p2_horiz < m_last_p2_horiz)
	{
		if ((m_last_p2_horiz - p2_horiz) > 128)
			m_dir0 = 0;
		else
			m_dir0 = 0x10;
	}

	/* P2 V DIR */
	if (p2_vert > m_last_p2_vert)
	{
		if ((p2_vert - m_last_p2_vert) > 128)
			m_dir1 = 0;
		else
			m_dir1 = 0x20;
	}
	else if (p2_vert < m_last_p2_vert)
	{
		if ((m_last_p2_vert - p2_vert) > 128)
			m_dir1 = 0x20;
		else
			m_dir1 = 0;
	}

	m_last_p1_horiz = p1_horiz;
	m_last_p1_vert  = p1_vert;
	m_last_p2_horiz = p2_horiz;
	m_last_p2_vert  = p2_vert;

	/* D0-D3 = Plyr 1 Horiz, D4-D7 = Plyr 1 Vert */
	if ((m_ld1) & (m_ld2))
	{
		return ((p1_horiz & 0x0f) | ((p1_vert << 4) & 0xf0));
	}
	/* D0-D3 = Plyr 2 Horiz, D4-D7 = Plyr 2 Vert */
	else if (m_ld2)
	{
		return ((p2_horiz & 0x0f) | ((p2_vert << 4) & 0xf0));
	}
	else
	{
		temp = ioport("IN0")->read() & 0x0f;

		return (temp | m_dir0 | m_dir1 | m_dir2 | m_dir3);
	}
}
