// license:BSD-3-Clause
// copyright-holders:David Haywood

#include "emu.h"
#include "cpu/arm7/arm7.h"
#include "cpu/arm7/arm7core.h"
#include "machine/nvram.h"
#include "pgmcrypt.h"
#include "sound/ics2115.h"
#include "emupal.h"
#include "screen.h"
#include "speaker.h"


namespace {

class igs_fear_state : public driver_device
{
public:
	igs_fear_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_gfxdecode(*this, "gfxdecode"),
		m_palette(*this, "palette") { }

	void igs_fear(machine_config &config);

	void init_igs_fear();

protected:
	virtual void video_start() override;

private:
	void sound_irq(int state);
	uint32_t screen_update_igs_fear(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	void pgm_create_dummy_internal_arm_region();
	required_device<cpu_device> m_maincpu;
	optional_device<gfxdecode_device> m_gfxdecode;
	required_device<palette_device> m_palette;
	void igs_igs_fear_map(address_map &map);
};


void igs_fear_state::video_start()
{
}


uint32_t igs_fear_state::screen_update_igs_fear(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	return 0;
}

void igs_fear_state::igs_igs_fear_map(address_map &map)
{
	map(0x00000000, 0x00003fff).rom(); /* Internal ROM */
	map(0x08000000, 0x0807ffff).rom().region("user1", 0);/* Game ROM */
}

// debugging only, not tile based
static const gfx_layout fearlayout =
{
	16,16,
	RGN_FRAC(1,1),
	7,              /* 7 bits per pixel - bit 0 unused in the roms we have*/
	{ 1,2,3,4,5,6,7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8, 8*8, 9*8, 10*8, 11*8, 12*8, 13*8, 14*8, 15*8},
	{ 0*128,1*128,2*128,3*128,4*128,5*128,6*128,7*128, 8*128,9*128,10*128,11*128,12*128,13*128,14*128,15*128 },
	128*16
};


static GFXDECODE_START( gfx_igs_fear )
	GFXDECODE_ENTRY( "gfx1", 0, fearlayout,   0, 16  )
	GFXDECODE_ENTRY( "gfx2", 0, fearlayout,   0, 16  )
	GFXDECODE_ENTRY( "gfx3", 0, fearlayout,   0, 16  )
GFXDECODE_END

static INPUT_PORTS_START( fear )
INPUT_PORTS_END

void igs_fear_state::sound_irq(int state)
{
}


void igs_fear_state::igs_fear(machine_config &config)
{
	ARM7(config, m_maincpu, 50000000/2);
	m_maincpu->set_addrmap(AS_PROGRAM, &igs_fear_state::igs_igs_fear_map);

	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_RASTER));
	screen.set_refresh_hz(60);
	screen.set_vblank_time(ATTOSECONDS_IN_USEC(0));
	screen.set_size(512, 256);
	screen.set_visarea(0, 512-1, 0, 256-1);
	screen.set_screen_update(FUNC(igs_fear_state::screen_update_igs_fear));
	screen.set_palette(m_palette);

	PALETTE(config, m_palette).set_entries(0x200);
	GFXDECODE(config, m_gfxdecode, m_palette, gfx_igs_fear);

	/* sound hardware */
	SPEAKER(config, "mono").front_center();
	ics2115_device &ics(ICS2115(config, "ics", 33.8688_MHz_XTAL)); // TODO : Correct?
	ics.irq().set(FUNC(igs_fear_state::sound_irq));
	ics.add_route(ALL_OUTPUTS, "mono", 5.0);
}


ROM_START( fearless )
	ROM_REGION( 0x04000, "maincpu", 0 )
	/* Internal rom of IGS027A ARM based MCU */
	ROM_LOAD( "fearless_igs027a", 0x00000, 0x4000, NO_DUMP ) // sticker marked 'F1'

	ROM_REGION32_LE( 0x80000, "user1", 0 ) // external ARM data / prg
	ROM_LOAD( "fearlessp_v-101us.u37", 0x000000, 0x80000, CRC(2522873c) SHA1(8db709877311b6d2796353fc9a44a820937e35c2) )

	ROM_REGION( 0x10000, "plcc", 0 )
	ROM_LOAD( "fearlessp_07.u33", 0x000000, 0x10000, CRC(7dae4900) SHA1(bbf7ba7c9e95ff2ffeb1dc0fc7ccedd4da274d01) ) // MX10EXAQC (80C51 XA based MCU) marked 07, not read protected?

	// these all report FIXED BITS (0xxxxxxx), hopefully just 7-bit colour data, non-tile based sprites
	ROM_REGION( 0x1000000, "gfx3", 0 )
	ROM_LOAD16_BYTE( "fearlessp_u6_cg-0h.u6",   0x000001, 0x800000, CRC(02d8bbbf) SHA1(7cf36c909a5d76096a725ffe0a697bcbafbcf985) )
	ROM_LOAD16_BYTE( "fearlessp_u7_cg-0l.u7",   0x000000, 0x800000, CRC(ca254db4) SHA1(f5670c2ff0720c84c9aff3cea95b118b6044e469) )
	ROM_REGION( 0x1000000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "fearlessp_u13_cg-1h.u13", 0x000001, 0x800000, CRC(c9d2a56d) SHA1(91d1665623bca743f68f15a27bbf433e2ffc0808) )
	ROM_LOAD16_BYTE( "fearlessp_u14_cg-1l.u14", 0x000000, 0x800000, CRC(7fe312d2) SHA1(c0add22d9fc4c0e32a03922cb709b947bfff429d) )
	ROM_REGION( 0x1000000, "gfx1", 0 )
	ROM_LOAD16_BYTE( "fearlessp_u17_cg-2h.u17", 0x000001, 0x800000, CRC(756fe1f2) SHA1(48ee81c5fa4808406b57b2521b836db3ff5a7fa9) )
	ROM_LOAD16_BYTE( "fearlessp_u18_cg-2l.u18", 0x000000, 0x800000, CRC(07623d66) SHA1(041d5e44917bc16caa720ea98bdc0a4f5fb4b8e0) )


	ROM_REGION( 0x800000, "ics", 0 )
	ROM_LOAD( "fearlessp_u25_music0.u25", 0x000000, 0x400000, CRC(a015b9b1) SHA1(7b129c59acd523dec82e58a75d873bbc5341fb28) )
	ROM_LOAD( "fearlessp_u26_music1.u26", 0x400000, 0x400000, CRC(9d5f18da) SHA1(42e5224c1af0898cc2e02b2e051ea8b629d5fb6d) )
ROM_END



void igs_fear_state::pgm_create_dummy_internal_arm_region()
{
	uint16_t *temp16 = (uint16_t *)memregion("maincpu")->base();

	// fill with RX 14
	int i;
	for (i=0;i<0x4000/2;i+=2)
	{
		temp16[i] = 0xff1e;
		temp16[i+1] = 0xe12f;

	}

	// jump straight to external area
	temp16[(0x0000)/2] = 0xd088;
	temp16[(0x0002)/2] = 0xe59f;
	temp16[(0x0004)/2] = 0x0680;
	temp16[(0x0006)/2] = 0xe3a0;
	temp16[(0x0008)/2] = 0xff10;
	temp16[(0x000a)/2] = 0xe12f;
	temp16[(0x0090)/2] = 0x0400;
	temp16[(0x0092)/2] = 0x1000;
}




void igs_fear_state::init_igs_fear()
{
	fearless_decrypt(machine());
	//sdwx_gfx_decrypt(machine());
	pgm_create_dummy_internal_arm_region();
}

} // anonymous namespace


GAME( 2006, fearless, 0, igs_fear, fear, igs_fear_state, init_igs_fear, ROT0, "IGS", "Fearless Pinocchio (V101US)",   MACHINE_IS_SKELETON )
