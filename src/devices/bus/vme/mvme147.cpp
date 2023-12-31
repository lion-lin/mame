// license:BSD-3-Clause
// copyright-holders:Joakim Larsson Edstrom
/***************************************************************************
 *
 *  Motorola MVME series of CPU boards: MVME-147
 *
 *  09/03/2016
 *
 * Thanks to Plamen Mihaylov and his site http://www.m88k.com/ I got the information
 * required to start the work with this driver. The goal is to boot a tape through
 * the MVME-350 device connected over a VME bus device.
 *
 *
 *       ||
 * ||    ||  MVME-147
 * ||||--||_____________________________________________________________
 * ||||--||      +--------------+ +-------+  +--------------+           |
 * ||    ||      | U22 147bug   | | FPU   |  | VMEchip      |         _ |__
 * Red   C| DS1  |              | | 68882 |  | ASIC         |        | |   |
 *  FAIL ||      +--------------+ |       |  |              |        | |   |
 * YellowC| DS2  | U30 147 bug  | +-------+  |              |        | |   |
 *  STAT ||      |              |            |              |        | |   |
 * Green C| DS3  +--------------+            |              |        | |   |
 *  RUN  ||      |              |            +--------------+        | |   |
 * Green C| DS4  |              |           +----------------+       | |VME|
 *  SCON ||      +--------------+           |  FLEX 10K30AQ  |       | |   |
 * RMT RST= J4   |              |+---------+|   PLD          |       | |P1 |
 *       ||      |              ||         ||                |       | |   |
 * ABRT O== S1   +--------------+|         ||                |       | |   |
 *       ||                      |         ||                |       | |   |
 * RSET O== S2                   |         ||                |       | |   |
 *       ||                      +---------+|                |       | |   |
 *       ||                                 +----------------+       | |   |
 *       ||                                                          |_|   |
 *       ||                      +---------+                           |___|
 *       ||                      |         |                           |
 *       ||                      |         |                           |
 *       ||                      |         |                           |
 *       ||                      |         |                           |
 *       ||                      +---------+                           |
 *       ||                                                            |
 *       ||                                                            |
 *       ||                                                            |
 *       ||                                                            |___
 *       ||                                                           _|   |
 *       ||                                                          | |   |
 *       ||                                 +------------+           | |   |
 *       ||                                 |            |           | |   |
 *       ||                                 |            |           | |   |
 *       ||                                 | 7990       |           | |VME|
 *       ||                                 |            |           | |   |
 *       ||                                 |            |           | |P2 |
 *       ||                                 +------------+           | |   |
 *       ||                                                          | |   |
 *       ||                                 +--------+               | |   |
 *       ||                                 |        |               | |   |
 *       ||                                 | 85C30  |               | |   |
 *       ||                                 |        |               | |   |
 *       ||                                 +--------+               | |   |
 *       ||                                 +--------+               | |   |
 *       ||                                 |        |               |_|   |
 *       ||                                 | 85C30  |                 |___|
 * ||    ||                                 |        |                   +
 * ||||--||                                 +--------+                   |
 * ||||--||--------------------------------------------------------------+
 * ||
 *
 * History of Motorola VME division (https://en.wikipedia.org/wiki/VMEbus)
 *---------------------------------
 * When Motorola released the 68000 processor 1979 the ambition of the deisgners
 * was also to standardize a versatile CPU bus to be able to build computer
 * systems without constructing PCB:s from scratch. This become VersaBus but the
 * boards was really too big and the computer world already saw the systems shrink
 * in size. Motorola's design center in Munich proposed to use the smaller and
 * already used Euroboard form factor and call it Versabus-E. This later became
 * VME which was standardized in the VITA organization 1981
 *
 * Misc links about Motorola VME division and this board:
 * http://bitsavers.trailing-edge.com/pdf/motorola/_dataBooks/1987_Microcomputer_Systems_and_Components.pdf
 * http://www.m88k.com/mvme147.html
 *
 * Description(s)
 * -------------
 * MVME-147 has the following feature set
 *      -  16, 25, or 33.33 MHz MC68030 enhanced 32-bit microprocessor
 *      -  16, 25, or 33.33 MHz MC68882 floating-point coprocessor
 *      -  4, 8, 16, or 32MB of shared DRAM, with  programmable parity
 *      -  4K x 8 SRAM and time-of-day clock with battery backup (MK48T12)
 *      -  Four 28/32-pin ROM/PROM/EPROM/EEPROM sockets, 16 bits wide
 *      -  Double High (6U) VMEmodule
 *      -  A32/D32 VMEbus master/slave interface with system controller function (VMEchip ASIC)
 *      -  Four EIA-232-D serial communications ports (85C30 x 2)
 *      -  Centronics compatible printer port ( PCC ASIC )
 *      -  Two 16-bit timers and watchdog timer (PCC ASIC )
 *      -  SCSI bus interface with DMA (WD33C93)
 *      -  Ethernet transceiver interface (AM7990)
 *      -  4-level requester, 7-level interrupter, and 7-level interrupt handler for VMEbus (VMEchip ASIC)
 *
 * NOTE: This driver currently mimics the MVME147SRF configuration: 16MHz, 4Mb RAM, no parity, no ethernet (See TODO)
 *
 * Address Map
 * --------------------------------------------------------------------------
 *                          Decscription
 * --------------------------------------------------------------------------
 * 00000000                 Boot ROM (4 cycles)/Onboard DRAM + VMEbus A32/A24 D32
 * F0000000                 VMEbus (A24 D16)
 * F1000000                 VMEbus (A32 D16)
 * FF800000                 ROM/EEPROM bank 1 D16
 * FFA00000                 ROM/EEPROM bank 2 D16
 * FFC00000                 reserved
 * ------------------------ Local I/O devices D8/D16/D32
 * FFFE0000-FFFE07F7        BB RAM D8 2040 bytes
 * FFFE07F8-FFFE07FF        BB TOD clock D8 8 bytes
 * FFFE1000-FFFE100F        PCC 32-bit registers D32 16 bytes
 * FFFE1010-FFFE102F        PCC 16-bit registers D16 32 bytes
 * FFFE1800-FFFE1803        LANCE (AM7990) D16 4 bytes
 * FFFE2000-FFFE201F        VMEchip registers D16 32 bytes
 * FFFE2800                 Printer data (write only) D8
 * FFFE2800                 Printer status (read only) D8
 * FFFE3000-FFFE3001        Serial 2 D8 2 bytes - 85C30
 * FFFE3002-FFFE3003        Serial 1 D8 2 bytes
 * FFFE3800-FFFE3801        Serial 4 D8 2 bytes - 85C30
 * FFFE3802-FFFE3803        Serial 3 D8 2 bytes
 * FFFE4000-FFFE401F        SCSI registers (WD33C93) D8 32 bytes
 * FFE50000                 reserved
 * FFFF0000                 VMEbux short I/O D16
 * --------------------------------------------------------------------------
 *
 * Interrupt sources MVME
 * ----------------------------------------------------------
 * Description                  Device  Lvl  IRQ    VME board
 *                           /Board      Vector  Address
 * ----------------------------------------------------------
 * On board Sources
 *
 * Off board Sources (other VME boards)
 *
 * ----------------------------------------------------------
 *
 * DMAC Channel Assignments
 * ----------------------------------------------------------
 * Channel         MVME147
 * ----------------------------------------------------------
 *
 *
 *  TODO:
 *  - Pass 147bug bootup tests
 *  - Add VME bus driver
 *  - Hook up MVME350
 *  - Get a tape file with a bootable data on it.
 *  - Add variants of boards, preferably as runtime configurations
 *
 ****************************************************************************/

#include "emu.h"
#include "mvme147.h"

#include "cpu/m68000/m68030.h"
#include "bus/rs232/rs232.h"
#include "machine/clock.h"
#include "machine/timekpr.h"

#define VERBOSE (0) // (LOG_GENERAL)
//#define LOG_OUTPUT_FUNC osd_printf_info
#include "logmacro.h"


#ifdef _MSC_VER
#define FUNCNAME __func__
#else
#define FUNCNAME __PRETTY_FUNCTION__
#endif


/* from documentation: http://www.m88k.com/Docs/147/147aih.pdf but crystal and divider not known */
#define BAUDGEN_CLOCK 5_MHz_XTAL
#define SCC_CLOCK (BAUDGEN_CLOCK) /* This gives prompt at the RS232 terminal device (9600) */


DEFINE_DEVICE_TYPE(VME_MVME147, vme_mvme147_card_device, "mvme147", "Motorola MVME147")

vme_mvme147_card_device::vme_mvme147_card_device(machine_config const &mconfig, char const *tag, device_t *owner, u32 clock)
	: device_t(mconfig, VME_MVME147, tag, owner, clock)
	, device_vme_card_interface(mconfig, *this)
	, m_maincpu (*this, "maincpu")
	, m_sccterm(*this, "scc")
	, m_sccterm2(*this, "scc2")
	, m_system(*this, "system")
	, m_ram(*this, "ram")
{
}

void vme_mvme147_card_device::mvme147_mem(address_map &map)
{
	map.unmap_value_high();
	map(0x00000008, 0x003fffff).ram().share("ram"); /* 4 Mb RAM */
	map(0xff800000, 0xff83ffff).rom().region("system", 0).mirror(0x1c0000); //mirror(0x00780000); /* ROM/EEPROM bank 1 - 147bug */
	//map(0xffa00000, 0xffbfffff).rom().region("user", 0); //mirror(0x00780000); /* ROM/EEPROM bank 2 - unpopulated */

		/*  SGS-Thompson M48T18 RAM and clock chip, only 4088 bytes used,  and 8 bytes for the RTC, out of 8Kb though */
	map(0xfffe0000, 0xfffe0fff).rw("m48t18", FUNC(timekeeper_device::read), FUNC(timekeeper_device::write));

	//map(0xfffe1000, 0xfffe100f).rw(FUNC(vme_mvme147_card_device::pcc32_r), FUNC(vme_mvme147_card_device::pcc32_w)); /* PCC 32 bits registers  - needs U64 cast defined to work */
	map(0xfffe1010, 0xfffe1017).rw(FUNC(vme_mvme147_card_device::pcc16_r), FUNC(vme_mvme147_card_device::pcc16_w)); /* PCC 16 bits registers */
	map(0xfffe1018, 0xfffe102f).rw(FUNC(vme_mvme147_card_device::pcc8_r), FUNC(vme_mvme147_card_device::pcc8_w)); /* PCC 8 bits registers */
	map(0xfffe2000, 0xfffe201b).rw(FUNC(vme_mvme147_card_device::vmechip_r), FUNC(vme_mvme147_card_device::vmechip_w)).umask32(0x00ff00ff); /* VMEchip 8 bits registers on odd adresses */

	map(0xfffe3000, 0xfffe3003).rw(m_sccterm, FUNC(scc85c30_device::ab_dc_r), FUNC(scc85c30_device::ab_dc_w)); /* Port 1&2 - Dual serial port Z80-SCC */
	map(0xfffe3800, 0xfffe3803).rw(m_sccterm2, FUNC(scc85c30_device::ab_dc_r), FUNC(scc85c30_device::ab_dc_w)); /* Port 3&4 - Dual serial port Z80-SCC */

	//map(0x100000, 0xfeffff).rw(FUNC(vme_mvme147_card_device::vme_a24_r), FUNC(vme_mvme147_card_device::vme_a24_w)); /* VMEbus Rev B addresses (24 bits) - not verified */
	//map(0xff0000, 0xffffff).rw(FUNC(vme_mvme147_card_device::vme_a16_r), FUNC(vme_mvme147_card_device::vme_a16_w)); /* VMEbus Rev B addresses (16 bits) - not verified */
}

/* Input ports */
static INPUT_PORTS_START (mvme147)
INPUT_PORTS_END

/* Start it up */
void vme_mvme147_card_device::device_start ()
{
	LOG("%s\n", FUNCNAME);

	m_genpurp_stat = 0x02; /* Indicate power up reset */
	m_vc_cntl_conf = 0x01; /* We are the system controller */
}

void vme_mvme147_card_device::device_reset ()
{
	LOG("%s\n", FUNCNAME);

	m_maincpu->space(AS_PROGRAM).install_rom(0, m_system->bytes() - 1, m_system->base());

	m_boot_mph = m_maincpu->space(AS_PROGRAM).install_read_tap(0x000004, 0x000007, "boot",
		[this](offs_t offset, uint32_t &data, uint32_t mem_mask)
		{
			if (!machine().side_effects_disabled() && ACCESSING_BITS_0_15)
			{
				m_maincpu->space(AS_PROGRAM).install_ram(0, m_ram.bytes() - 1, m_ram.target());
				m_boot_mph.remove();
			}
		});

	m_genpurp_stat &= 0xfe; /* Clear parity error bit - not used by MAME at this point so just for the record */
}

/****
 * PCC - Peripheral Channel Controller driver, might deserve its own driver but will rest here until another board wants it
 */
#if 0 /* Doesn't compile atm */
uint32_t vme_mvme147_card_device::pcc32_r(offs_t offset){
	LOG("%s[%04x]", FUNCNAME, offset);
	switch(offset)
	{
	default:
		LOG(" - unsupported register\n");
	}
	return 0x00;
}

void vme_mvme147_card_device::pcc32_w(offs_t offset, uint32_t data){
	LOG("%s[%04x]= %08lx", FUNCNAME, offset, data);
	switch(offset)
	{
	default:
		LOG("- unsupported register\n");
	}
}
#endif

#define P16BASE 0xfffe1010
#define P16_TIMER1_PRELOAD (P16BASE)
#define P16_TIMER1_COUNT   (P16BASE + 2)
#define P16_TIMER2_PRELOAD (P16BASE + 4)
#define P16_TIMER2_COUNT   (P16BASE + 6)

uint16_t vme_mvme147_card_device::pcc16_r(offs_t offset){
	uint16_t ret = 0;

	LOG("Call to %s[%04x]", FUNCNAME, offset);
	switch(offset)
	{
	case  P16_TIMER1_PRELOAD - P16BASE   : LOG(" -> %02x Timer 1 preload - not implemented\n", ret); break;
	case  P16_TIMER1_COUNT   - P16BASE   : LOG(" -> %02x Timer 1 count - not implemented\n", ret); break;
	case  P16_TIMER2_PRELOAD - P16BASE   : LOG(" -> %02x Timer 2 preload - not implemented\n", ret); break;
	case  P16_TIMER2_COUNT   - P16BASE   : LOG(" -> %02x Timer 2 count - not implemented\n", ret); break;
	default:
		LOG(" -> %02x unsupported register\n", ret);
	}
	return ret;
}

void vme_mvme147_card_device::pcc16_w(offs_t offset, uint16_t data){
	LOG("Call to %s[%04x] <- %04x - ", FUNCNAME, offset, data);
	switch(offset)
	{
	case    P16_TIMER1_PRELOAD - P16BASE   : LOG("Timer 1 preload - not implemented\n"); break;
	case    P16_TIMER1_COUNT   - P16BASE   : LOG("Timer 1 count - not implemented\n"); break;
	case    P16_TIMER2_PRELOAD - P16BASE   : LOG("Timer 2 preload - not implemented\n"); break;
	case    P16_TIMER2_COUNT   - P16BASE   : LOG("Timer 2 count - not implemented\n"); break;
	default:
	LOG("unsupported register\n");
	}
}

#define P8BASE 0xfffe1018
#define P8_TIMER1_INT_CNTL  0xfffe1018
#define P8_TIMER1_CNTL      0xfffe1019
#define P8_TIMER2_INT_CNTL  0xfffe101A
#define P8_TIMER2_CNTL      0xfffe101B
#define P8_ACFAIL_INT_CNTL  0xfffe101C
#define P8_WDOG_TIMER_CNTL  0xfffe101D
#define P8_PRINTER_INT_CNTL 0xfffe101E
#define P8_PRINTER_CNTL     0xfffe101F
#define P8_DMA_INT_CNTL     0xfffe1020
#define P8_DMA_CNTL_STAT    0xfffe1021
#define P8_BUSERR_CNTL      0xfffe1022
#define P8_DMA_STATUS       0xfffe1023
#define P8_ABORT_INT_CNTL   0xfffe1024
#define P8_TABADD_FC_CNTL   0xfffe1025
#define P8_SERIAL_INT_CNTL  0xfffe1026
#define P8_GEN_PURP_CNTL    0xfffe1027
#define P8_LAN_INT_CNTL     0xfffe1028
#define P8_GEN_PURP_STAT    0xfffe1029
#define P8_SCSI_INT_CNTL    0xfffe102A
#define P8_SLAVE_BASE_ADDR  0xfffe102B
#define P8_SWI_1_CNTL       0xfffe102C
#define P8_INT_VECT_BASE    0xfffe102D
#define P8_SWI_2_CNTL       0xfffe102E
#define P8_REVISION_LEVEL   0xfffe102F
#define P8_PRINTER_DATA     0xfffe2800
#define P8_PRINTER_STATUS   0xfffe2800

uint8_t vme_mvme147_card_device::pcc8_r(offs_t offset){
	uint8_t ret = 0;

	LOG("Call to %s[%04x]      ", FUNCNAME, offset);
	switch(offset + P8BASE)
	{
	case P8_TIMER1_INT_CNTL     : LOG(" -> %02x    - Timer 1 Interrupt Control  - not implemented\n", ret); break;
	case P8_TIMER1_CNTL         : LOG(" -> %02x    - Timer 1 Control - not implemented\n", ret); break;
	case P8_TIMER2_INT_CNTL     : LOG(" -> %02x    - Timer 2 Interrupt Control - not implemented\n", ret); break;
	case P8_TIMER2_CNTL         : LOG(" -> %02x    - Timer 2 Control - not implemented\n", ret); break;
	case P8_ACFAIL_INT_CNTL     : LOG(" -> %02x    - AC Fail Interrupt Control Register - not implemented\n", ret); break;
	case P8_WDOG_TIMER_CNTL     : LOG(" -> %02x    - Watchdog Timer Control Register - not implemented\n", ret); break;
	case P8_PRINTER_INT_CNTL    : LOG(" -> %02x    - Printer Interrupt Control Register - not implemented\n", ret); break;
	case P8_PRINTER_CNTL        : LOG(" -> %02x    - Printer Control Register - not implemented\n", ret); break;
	case P8_DMA_INT_CNTL        : LOG(" -> %02x    - DMA Interrupt Control Register - not implemented\n", ret); break;
	case P8_DMA_CNTL_STAT       : LOG(" -> %02x    - DMA Control and Status Register - not implemented\n", ret); break;
	case P8_BUSERR_CNTL         : LOG(" -> %02x    - Bus Error Interrupt Control Register - not implemented\n", ret); break;
	case P8_DMA_STATUS          : LOG(" -> %02x    - DMA Status Register - not implemented\n", ret); break;
	case P8_ABORT_INT_CNTL      : LOG(" -> %02x    - Abort Interrupt Control Register - not fully implemented\n", ret);
		/* Bit 3 When this bit is high, the interrupt is enabled. The interrupt is disabled when this bit is low. This bit is cleared by reset.
		 Bit 6 This bit indicates the current state of the ABORT switch. When this bit is low, the ABORT switch is not pressed. When this bit is
		   high, the ABORT switch is pressed.
		 Bit 7 When this bit is high, an abort interrupt is being generated at Level 7. This bit is edge sensitive and it is set on the leading
		   edge of interrupt enable and abort. This bit is cleared when a 1 is written to it or when the interrupt is disabled. When cleared,
		   it remains cleared until the next leading edge of interrupt enable and abort. This bit is cleared by reset. */
		ret = 0; /* Always return reset values for now */
		break;
	case P8_TABADD_FC_CNTL   : LOG(" -> %02x    - Table Address Function Code Register - not implemented\n", ret); break;
	case P8_SERIAL_INT_CNTL  : LOG(" -> %02x    - Serial Port Interrupt Control Register - not implemented\n", ret); break;
	case P8_GEN_PURP_CNTL    : LOG(" -> %02x    - General Purpose Control Register - not implemented\n", ret); break;
	case P8_LAN_INT_CNTL     : LOG(" -> %02x    - LAN Interrupt Control Register - not implemented\n", ret); break;
	case P8_GEN_PURP_STAT    : LOG(" -> %02x    - General Purpose Status Register\n", ret);
		ret = m_genpurp_stat;
		break;
	case P8_SCSI_INT_CNTL   : LOG(" -> %02x    - SCSI Port Interrupt Control Register - not implemented\n", ret); break;
	case P8_SLAVE_BASE_ADDR : LOG(" -> %02x    - Slave Base Address Register - not implemented\n", ret); break;
	case P8_SWI_1_CNTL      : LOG(" -> %02x    - Software Interrupt 1 Control Register - not implemented\n", ret); break;
	case P8_INT_VECT_BASE   : LOG(" -> %02x    - Interrupt Vector Base - not implemented\n", ret); break;
	case P8_SWI_2_CNTL      : LOG(" -> %02x    - Software Interrupt 2 Control Register - not implemented\n", ret); break;
	case P8_REVISION_LEVEL  : LOG(" -> %02x    - PCC Revision Level Register - not implemented\n", ret); break;
	case P8_PRINTER_STATUS  : LOG(" -> %02x    - Printer Status Register - not implemented\n", ret); break;
	default:
		LOG(" -> %02x    - unsupported register\n", ret);
	}
	return ret;
}

void vme_mvme147_card_device::pcc8_w(offs_t offset, uint8_t data){
	LOG("Call to %s[%04x] <- %02x    - ", FUNCNAME, offset, data);
	switch(offset + P8BASE)
	{
	case P8_TIMER1_INT_CNTL : LOG("Timer 1 Interrupt Control - not implemented\n"); break;
	case P8_TIMER1_CNTL     : LOG("Timer 1 Control - not implemented\n"); break;
	case P8_TIMER2_INT_CNTL : LOG("Timer 2 Interrupt Control - not implemented\n"); break;
	case P8_TIMER2_CNTL     : LOG("Timer 2 Control - not implemented\n"); break;
	case P8_ACFAIL_INT_CNTL : LOG("AC Fail Interrupt Control Register - not implemented\n"); break;
	case P8_WDOG_TIMER_CNTL : LOG("Watchdog Timer Control Register - not implemented\n"); break;
	case P8_PRINTER_INT_CNTL: LOG("Printer Interrupt Control Register - not implemented\n"); break;
	case P8_PRINTER_CNTL    : LOG("Printer Control Register - not implemented\n"); break;
	case P8_DMA_INT_CNTL    : LOG("DMA Interrupt Control Register - not implemented\n"); break;
	case P8_DMA_CNTL_STAT   : LOG("DMA Control and Status Register - not implemented\n"); break;
	case P8_BUSERR_CNTL     : LOG("Bus Error Interrupt Control Register - not implemented\n"); break;
	case P8_DMA_STATUS      : LOG("DMA Status Register - not implemented\n"); break;
	case P8_ABORT_INT_CNTL  : LOG("Abort Interrupt Control Register - not implemented\n"); break;
	case P8_TABADD_FC_CNTL  : LOG("Table Address Function Code Register - not implemented\n"); break;
	case P8_SERIAL_INT_CNTL : LOG("Serial Port Interrupt Control Register - not implemented\n"); break;
	case P8_GEN_PURP_CNTL   : LOG("General Purpose Control Register - not implemented\n");
		/*Bits 0-1 These bits control local RAM parity checking. These bits should not be enabled on the MVME147-010.
		     These bits are cleared by reset. x0 = parity disabled, x1 = parity enabled
		Bit  2   This bit is used to test the parity generating and checking logic. When this bit is low, correct parity is written to the DRAM;
		     when high, incorrect parity is written to the DRAM. This bit is cleared by reset.
		 NOTE: We really don't care about DRAM parity!
		Bit  3   When set, this bit is used to enable the local bus timer that is part of the PCC. Because the VMEchip also contains a local bus
		     timer, this bit should be cleared, turning off the PCC local bus timer. This bit is cleared by reset.
		Bit  4   This bit is the master interrupt enable. When this bit is low, all interrupts on the MVME147 are disabled; when high, all
		     interrupts are enabled. This bit is cleared by reset
		Bits 5-7 When the pattern %101 is written to these bits, the front panel RESET switch is disabled. The RESET switch is enabled for any
		     other pattern. These bits are cleared by reset.
		 TODO: Bit 4-7 needs to be implemented
		*/
		break;
	case P8_LAN_INT_CNTL     : LOG("LAN Interrupt Control Register - not implemented\n"); break;
	case P8_GEN_PURP_STAT    : LOG("General Purpose Status Register\n");
		/* Bit 0 This bit is set when a parity error occurs while the local processor is accessing RAM. This bit is cleared by writing a 1 to it.
		   This bit is cleared by reset.
		 Bit 1 This bit is set when a power-up reset occurs. It is cleared by writing a 1 to it.
		   When the MVME147BUG is installed, its initialization code clears this bit.
		*/
		m_genpurp_stat &= ((data & 1) ? ~1 : 0xff); // Check if parity error bit needs to be cleared
		m_genpurp_stat &= ((data & 2) ? ~2 : 0xff); // Check if power up reset bit needs to be cleared
		break;
	case P8_SCSI_INT_CNTL   : LOG("SCSI Port Interrupt Control Register - not implemented\n"); break;
	case P8_SLAVE_BASE_ADDR : LOG("Slave Base Address Register - not implemented\n"); break;
	case P8_SWI_1_CNTL      : LOG("Software Interrupt 1 Control Register - not implemented\n"); break;
	case P8_INT_VECT_BASE   : LOG("Interrupt Vector Base - not implemented\n"); break;
	case P8_SWI_2_CNTL      : LOG("Software Interrupt 2 Control Register - not implemented\n"); break;
	case P8_REVISION_LEVEL  : LOG("PCC Revision Level Register - not implemented\n"); break;
	case P8_PRINTER_DATA    : LOG("Printer Data Register - not implemented\n"); break;
	default:
		LOG("unsupported register\n");
	}
}

/*
 * VMEchip ASIC driver, might deserve its own driver but will rest here until another board wants it
 */
#define VCBASE 0xfffe2001
#define VC_SYS_CNTL_CONF    0xfffe2001
#define VC_VMEBUS_REQ_CONF  0xfffe2003
#define VC_MASTER_CONF      0xfffe2005
#define VC_SLAVE_CONF       0xfffe2007
#define VC_TIMER_CONF       0xfffe2009
#define VC_SLAVE_ADR_MOD    0xfffe200B
#define VC_MASTER_ADR_MOD   0xfffe200D
#define VC_INT_HNDL_MASK    0xfffe200F
#define VC_UTIL_INT_MASK    0xfffe2011
#define VC_UTIL_INT_VECT    0xfffe2013
#define VC_INT_REQUEST      0xfffe2015
#define VC_VMEBUS_STAT_ID   0xfffe2017
#define VC_BUS_ERR_STATUS   0xfffe2019
#define VC_GCSR_BASE_ADR    0xfffe201B


uint8_t vme_mvme147_card_device::vmechip_r(offs_t offset){
	uint8_t ret = 0;

	LOG("Call to %s[%04x]      ", FUNCNAME, offset);
	switch(offset * 2 + VCBASE)
	{
	case VC_SYS_CNTL_CONF    :
		LOG(" -> %02x - System Controller Configuration Register - not implemented\n", ret);
		ret = m_vc_cntl_conf;
		break;
	case VC_VMEBUS_REQ_CONF  :
		/*
		Bits 0-1 These control bits configure the VMEbus requester level as shown in the table below:
		     RQLEV1 RQLEV0 Level
		      0      0      0
		      0      1      1
		      1      0      2
		      1      1      3
		  These bits are set to 1, 1 by any reset. Note that writes to REQLEV1,0 do not change the actual
		   requester level until the MVME147 goes through the action of having VMEbus mastership and releasing it. This means that
		   there are times when the value written into REQLEV1,0 do not match the current requester level (the request level is lagging).
		   During such times, reads to REQLEV1,0 reflect the actual requester level, not the value written into REQLEV1,0.
		Bit  3   Setting this bit to 1 prevents the requester from releasing the VMEbus. However, unlike the DWB control bit, setting the
		   RNEVER bit does not cause the requester to request the VMEbus. Clearing the RNEVER bit allows the requester to
		   relinquish the VMEbus in accordance with the other control bits of the requester configuration register.
		   This bit is cleared by any reset.
		Bit  4   The RWD bit allows software to configure the requester release mode. When the bit is set, if RNEVER and DWB are both
		   cleared to 0, the requester releases the VMEbus after the MC68030 completes a VMEbus cycle. When the bit is cleared, if
		   RNEVER and DWB are both cleared to 0, the requester operates in the Release-On-Request (ROR) mode. After acquiring control
		   of the VMEbus, it maintains control until it detects another request pending on the VMEbus. This bit is cleared by any reset.
		Bit  5   The RONR bit controls the manner in which the VMEchip requests the VMEbus. When the bit is set; anytime the
		   MVME147 has bus mastership, then gives it up, the VMEchip does not request the VMEbus again until it detects the bus
		   request signal BR*, on its level, negated for at least 150 ns. When the VMEchip detects BR* negated, it refrains from
		   driving it again for at least 200 ns. This bit is cleared by any reset.
		Bit 6    The DHB status bit is 1 when the MVME147 is VMEbus master and 0 when it is not.
		Bit 7    Setting the DWB control bit to 1 causes the VMEchip to request the VMEbus (if not already bus master). When VMEbus
		   mastership has been obtained, it is not relinquished until after the DWB and RNEVER bits are both cleared.
		   This bit is cleared by any reset.
	*/
		ret = 1 << 6; /* Let BUG147 think we are bus master. TODO: Implement proper VME bus signalling */
		LOG(" -> %02x - VMEbus Requester Configuration Register - not implemented\n", ret);
		break;
	case VC_MASTER_CONF  : LOG(" -> %02x - Master Configuration Register - not implemented\n", ret); break;
	case VC_SLAVE_CONF   : LOG(" -> %02x - Slave Configuration Register - not implemented\n", ret); break;
	case VC_TIMER_CONF   : LOG(" -> %02x - Timer Configuration Register - not implemented\n", ret);
	/*Bits 0-1 These two bits configure the local time-out period. They are set to 1 by any reset.
	       LBTO1 LBTO0 Time-Out Period
	    0     0    102 microseconds
	    0     1    205 microseconds
	    1     0    410 microseconds
	    1     1    Timer disabled
	       The local bus timer activates bus error to the MC68030 when it tries to access nonexistent locations in the local memory map
	  Bits 2-3 These two bits configure the VMEbus access time-out period. They are set to 1 by any reset.
	       ACTO1 ACTO0 Time-Out Period
	    0     0    102 microseconds
	    0     1    1.6 millisecond
	    1     0     51 milliseconds
	    1     1    Timer disabled
	       The VMEbus access timer activates bus error to the MC68030 (except on write posted time-outs) when the VMEchip is
	       unsuccessful in obtaining the VMEbus within the time-out period
	   Bits 4-5 These two bits configure the VMEbus global time-out period. VBTO1 is set to 1 and VBTO0 is cleared to 0 by SYSRESET.
	       VBTO1 VBTO0 Time-Out Period
	    0     0    102 microseconds
	    0     1    205 microseconds
	    1     0    410 microseconds
	    1     1    Timer disabled
	       The VMEbus global timer activates BERR* on the VMEbus.
	   Bit  6  Setting ARBTO to 1 enables the VMEbus arbitration timer. The VMEbus arbitration timer activates BBSY* if it is not activated
	       within 410 us after the MVME147 arbiter issues a bus grant. The timer deactivates BBSY* as specified in the VMEbus specification.
	       This causes the arbiter to arbitrate any pending requests for the bus. This bit is set to 1 by SYSRESET.
	*/
		break;
	case VC_SLAVE_ADR_MOD   : LOG(" -> %02x - Slave Address Modifier Register - not implemented\n", ret); break;
	case VC_MASTER_ADR_MOD  : LOG(" -> %02x - Master Address Modifier Register - not implemented\n", ret); break;
	case VC_INT_HNDL_MASK   : LOG(" -> %02x - Interrupt Handler Mask Register - not implemented\n", ret); break;
	case VC_UTIL_INT_MASK   : LOG(" -> %02x - Utility Interrupt Mask Register - not implemented\n", ret); break;
	case VC_UTIL_INT_VECT   : LOG(" -> %02x - Utility Interrupt Vector Register - not implemented\n", ret); break;
	case VC_INT_REQUEST     : LOG(" -> %02x - Interrupt Request Register - not implemented\n", ret); break;
	case VC_VMEBUS_STAT_ID  : LOG(" -> %02x - VMEbus Status/ID Register - not implemented\n", ret); break;
	case VC_BUS_ERR_STATUS  : LOG(" -> %02x - Bus Error Status Register - not implemented\n", ret); break;
	case VC_GCSR_BASE_ADR   : LOG(" -> %02x - GCSR Base Address Configuration Register - not implemented\n", ret); break;
	default:
		LOG("unsupported register");
	}
	return ret;
}

void vme_mvme147_card_device::vmechip_w(offs_t offset, uint8_t data){
	LOG("Call to %s[%04x] <- %02x - ", FUNCNAME, offset, data);
	switch(offset * 2 + VCBASE)
	{
	case VC_SYS_CNTL_CONF    :
		LOG("System Controller Configuration Register - not implemented\n");
		m_vc_cntl_conf = data & 0xff;
		break;
	case VC_VMEBUS_REQ_CONF : LOG("VMEbus Requester Configuration Register - not implemented\n"); break;
	case VC_MASTER_CONF     : LOG("Master Configuration Register - not implemented\n"); break;
	case VC_SLAVE_CONF      : LOG("Slave Configuration Register - not implemented\n"); break;
	case VC_TIMER_CONF      : LOG("Timer Configuration Register - not implemented\n"); break;
	case VC_SLAVE_ADR_MOD   : LOG("Slave Address Modifier Register - not implemented\n"); break;
	case VC_MASTER_ADR_MOD  : LOG("Master Address Modifier Register - not implemented\n"); break;
	case VC_INT_HNDL_MASK   : LOG("Interrupt Handler Mask Register - not implemented\n"); break;
	case VC_UTIL_INT_MASK   : LOG("Utility Interrupt Mask Register - not implemented\n"); break;
	case VC_UTIL_INT_VECT   : LOG("Utility Interrupt Vector Register - not implemented\n"); break;
	case VC_INT_REQUEST     : LOG("Interrupt Request Register - not implemented\n"); break;
	case VC_VMEBUS_STAT_ID  : LOG("VMEbus Status/ID Register - not implemented\n"); break;
	case VC_BUS_ERR_STATUS  : LOG("Bus Error Status Register - not implemented\n"); break;
	case VC_GCSR_BASE_ADR   : LOG("GCSR Base Address Configuration Register - not implemented\n"); break;
	default:
		LOG("unsupported register\n");
	}
}


#if 0
/* Dummy VME access methods until the VME bus device is ready for use */
uint16_t vme_mvme147_card_device::vme_a24_r(){
	LOG("%s\n", FUNCNAME);
	return (uint16_t) 0;
}

void vme_mvme147_card_device::vme_a24_w(uint16_t data){
	LOG("%s\n", FUNCNAME);
}

uint16_t vme_mvme147_card_device::vme_a16_r(){
	LOG("%s\n", FUNCNAME);
	return (uint16_t) 0;
}

void vme_mvme147_card_device::vme_a16_w(uint16_t data){
	LOG("%s\n", FUNCNAME);
}
#endif

/*
 * Machine configuration
 */
void vme_mvme147_card_device::device_add_mconfig(machine_config &config)
{
	/* basic machine hardware */
	M68030(config, m_maincpu, 16_MHz_XTAL);
	m_maincpu->set_addrmap(AS_PROGRAM, &vme_mvme147_card_device::mvme147_mem);

	M48T02(config, "m48t18", 0); /* t08 differs only in accepted voltage levels compared to t18 */

	/* Terminal Port config */
	SCC85C30(config, m_sccterm, SCC_CLOCK);
	m_sccterm->out_txda_callback().set("rs232trm", FUNC(rs232_port_device::write_txd));
	m_sccterm->out_dtra_callback().set("rs232trm", FUNC(rs232_port_device::write_dtr));
	m_sccterm->out_rtsa_callback().set("rs232trm", FUNC(rs232_port_device::write_rts));

	rs232_port_device &rs232trm(RS232_PORT(config, "rs232trm", default_rs232_devices, "terminal"));
	rs232trm.rxd_handler().set(m_sccterm, FUNC(scc85c30_device::rxa_w));
	rs232trm.cts_handler().set(m_sccterm, FUNC(scc85c30_device::ctsa_w));

	SCC85C30(config, m_sccterm2, SCC_CLOCK);
}

/* ROM definitions */
ROM_START (mvme147)
	ROM_REGION32_BE(0x40000, "system", 0)
	ROM_DEFAULT_BIOS("147bug-v2.44")

	ROM_SYSTEM_BIOS(0, "147bug-v2.44", "MVME147 147bug v2.44")
	ROMX_LOAD("147bug-2.44-u22.bin", 0x00000, 0x20000, CRC (da09ce8a) SHA1 (3eaa8fa802187d9b08f453ff1ba64f5113a195a9), ROM_SKIP(1) | ROM_BIOS(0))
	ROMX_LOAD("147bug-2.44-u30.bin", 0x00001, 0x20000, CRC (f883e17d) SHA1 (01fe43e5ddfd3cf8aabb5a5959c80a8b5ec5d895), ROM_SKIP(1) | ROM_BIOS(0))
/*
 * System ROM information
 *
 * 147bug version 2.44 is released 1999, coprighted by Motorola Inc from 1988
 *
 * 147bug SCC channel setup sequence
 *----------------------------------
 *  09 80 - Channel A reset
 *  04 44 - x16 clock, 1 stop bits, no parity
 *  03 C1 - 8 bit, receiver enable, auto enable off
 *  01 00 -
 *  02 70 - set interrupt vector 70
 *  05 EA
 *  0B 56
 *  0C 0E - low baudrate divider
 *  0D 00 - hi baudrate divider
 *  0E 83 - Baud Rate Generator (BRG) enabled, PCLK is BRG source, BRG is DPLL source
 *  06 00 - Tx sync character SDLC
 *  07 00 - Rx sync character SDLC
 *  0F 00 - disable all external interrupt and status
 *  10
 * channel B is identical but resets Channel B of course, SCC2 is also identical except using interrupt vector 71
 */

	ROM_SYSTEM_BIOS(1, "147bug-v2.43", "MVME147 147bug v2.43")
	ROMX_LOAD("5741b42e.bin", 0x00000, 0x20000, CRC (2ba98f97) SHA1 (5f18c6dd6a7b03067890f0164ef3d37ced907d7f), ROM_SKIP(1) | ROM_BIOS(1))
	ROMX_LOAD("5741b41e.bin", 0x00001, 0x20000, CRC (dfa014f2) SHA1 (ff9db90a05c295819ce7ca7c1a6ac67b04003728), ROM_SKIP(1) | ROM_BIOS(1))
/*
 * System ROM information
 *
 * 147bug version 2.43 is released 1992, coprighted by Motorola Inc from 1988
 *
 */

ROM_END

const tiny_rom_entry *vme_mvme147_card_device::device_rom_region() const
{
	return ROM_NAME(mvme147);
}

ioport_constructor vme_mvme147_card_device::device_input_ports() const
{
	return INPUT_PORTS_NAME(mvme147);
}

/* Driver */
//    YEAR  NAME     PARENT  COMPAT  MACHINE  INPUT    CLASS          INIT        COMPANY     FULLNAME    FLAGS
//COMP( 1989, mvme147, 0,      0,      mvme147, mvme147, vme_mvme147_card_device, empty_init, "Motorola", "MVME-147", MACHINE_NO_SOUND_HW )
