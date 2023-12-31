// license:BSD-3-Clause
// copyright-holders:Bryan McPhail, David Haywood
#include "emu.h"
#include "dynduke.h"

#include "screen.h"


/******************************************************************************/

void dynduke_state::background_w(offs_t offset, uint16_t data, uint16_t mem_mask)
{
	COMBINE_DATA(&m_back_data[offset]);
	m_bg_layer->mark_tile_dirty(offset);
}

void dynduke_state::foreground_w(offs_t offset, uint16_t data, uint16_t mem_mask)
{
	COMBINE_DATA(&m_fore_data[offset]);
	m_fg_layer->mark_tile_dirty(offset);
}

void dynduke_state::text_w(offs_t offset, uint16_t data, uint16_t mem_mask)
{
	COMBINE_DATA(&m_videoram[offset]);
	m_tx_layer->mark_tile_dirty(offset);
}

TILE_GET_INFO_MEMBER(dynduke_state::get_bg_tile_info)
{
	uint32_t tile=m_back_data[tile_index];
	uint32_t const color=tile >> 12;

	tile=tile&0xfff;

	tileinfo.set(1,
			tile+m_back_bankbase,
			color,
			0);
}

TILE_GET_INFO_MEMBER(dynduke_state::get_fg_tile_info)
{
	uint32_t tile=m_fore_data[tile_index];
	uint32_t const color=tile >> 12;

	tile=tile&0xfff;

	tileinfo.set(2,
			tile+m_fore_bankbase,
			color,
			0);
}

TILE_GET_INFO_MEMBER(dynduke_state::get_tx_tile_info)
{
	uint32_t tile = m_videoram[tile_index];
	uint32_t const color = (tile >> 8) & 0x0f;

	tile = (tile & 0xff) | ((tile & 0xc000) >> 6);

	tileinfo.set(0,
			tile,
			color,
			0);
}

void dynduke_state::video_start()
{
	m_bg_layer = &machine().tilemap().create(*m_gfxdecode, tilemap_get_info_delegate(*this, FUNC(dynduke_state::get_bg_tile_info)), TILEMAP_SCAN_COLS,16,16,32,32);
	m_fg_layer = &machine().tilemap().create(*m_gfxdecode, tilemap_get_info_delegate(*this, FUNC(dynduke_state::get_fg_tile_info)), TILEMAP_SCAN_COLS,16,16,32,32);
	m_tx_layer = &machine().tilemap().create(*m_gfxdecode, tilemap_get_info_delegate(*this, FUNC(dynduke_state::get_tx_tile_info)), TILEMAP_SCAN_ROWS, 8, 8,32,32);

	m_fg_layer->set_transparent_pen(15);
	m_tx_layer->set_transparent_pen(15);

	save_item(NAME(m_back_bankbase));
	save_item(NAME(m_fore_bankbase));
	save_item(NAME(m_back_enable));
	save_item(NAME(m_fore_enable));
	save_item(NAME(m_sprite_enable));
	save_item(NAME(m_txt_enable));
	save_item(NAME(m_old_back));
	save_item(NAME(m_old_fore));
}

void dynduke_state::gfxbank_w(offs_t offset, uint16_t data, uint16_t mem_mask)
{
	if (ACCESSING_BITS_0_7)
	{
		m_back_bankbase = BIT(data, 0) ? 0x1000 : 0;
		m_fore_bankbase = BIT(data, 4) ? 0x1000 : 0;

		if (m_back_bankbase!=m_old_back)
			m_bg_layer->mark_all_dirty();
		if (m_fore_bankbase!=m_old_fore)
			m_fg_layer->mark_all_dirty();

		m_old_back=m_back_bankbase;
		m_old_fore=m_fore_bankbase;
	}
}


void dynduke_state::control_w(offs_t offset, uint16_t data, uint16_t mem_mask)
{
	if (ACCESSING_BITS_0_7)
	{
		// bit 0x80 toggles, maybe sprite buffering?
		// bit 0x40 is flipscreen
		// bit 0x20 not used?
		// bit 0x10 not used?
		// bit 0x08 is set on the title screen (sprite disable?)
		// bit 0x04 unused? txt disable?
		// bit 0x02 is used on the map screen (fore disable?)
		// bit 0x01 set when inserting coin.. bg disable?

		m_back_enable = BIT(~data, 0);
		m_fore_enable = BIT(~data, 1);
		m_txt_enable = BIT(~data, 2);
		m_sprite_enable = BIT(~data, 3);

		flip_screen_set(BIT(data, 6));
	}
}

void dynduke_state::draw_sprites(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	if (!m_sprite_enable)
		return;

	uint16_t const *const buffered_spriteram16 = m_spriteram->buffer();

	constexpr uint32_t pri_mask[4] = {
		GFX_PMASK_8 | GFX_PMASK_4 | GFX_PMASK_2, // Untested: does anything use it? Could be behind background
		GFX_PMASK_8 | GFX_PMASK_4 | GFX_PMASK_2,
		GFX_PMASK_8 | GFX_PMASK_4,
		GFX_PMASK_8,
	};

	for (int offs = 0; offs < 0x800; offs += 4)
	{
		// Don't draw empty sprite table entries
		if ((buffered_spriteram16[offs + 3] >> 8) != 0xf)
			continue;

		uint32_t const pri = pri_mask[(buffered_spriteram16[offs + 2] >> 13) & 3];

		bool fx = BIT(buffered_spriteram16[offs + 0], 13);
		bool fy = BIT(buffered_spriteram16[offs + 0], 14);
		int32_t y = buffered_spriteram16[offs + 0] & 0xff;
		int32_t x = util::sext(buffered_spriteram16[offs + 2], 9);

		uint32_t const color = (buffered_spriteram16[offs + 0] >> 8) & 0x1f;
		uint32_t const sprite = buffered_spriteram16[offs + 1] & 0x3fff;

		if (flip_screen())
		{
			x = 240 - x;
			y = 240 - y;
			fx = !fx;
			fy = !fy;
		}

		m_gfxdecode->gfx(3)->prio_transpen(bitmap, cliprect,
				sprite,
				color, fx, fy, x, y,
				screen.priority(), pri, 15);
	}
}

void dynduke_state::draw_background(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect, int pri, uint32_t pri_mask)
{
	/* The transparency / palette handling on the background layer is very strange */
	bitmap_ind16 &bm = m_bg_layer->pixmap();
	/* if we're disabled, don't draw */
	if (!m_back_enable)
	{
		bitmap.fill(m_palette->black_pen(), cliprect);
		return;
	}

	int scrolly = ((m_scroll_ram[0x01] & 0x30) << 4) +((m_scroll_ram[0x02] & 0x7f) << 1) + ((m_scroll_ram[0x02] & 0x80) >> 7);
	int scrollx = ((m_scroll_ram[0x09] & 0x30) << 4) +((m_scroll_ram[0x0a] & 0x7f) << 1) + ((m_scroll_ram[0x0a] & 0x80) >> 7);

	if (flip_screen())
	{
		scrolly = 256 - scrolly;
		scrollx = 256 - scrollx;
	}

	for (int y=cliprect.top();y<=cliprect.bottom();y++)
	{
		int const realy = (y + scrolly) & 0x1ff;
		uint16_t const *const src = &bm.pix(realy);
		uint16_t *const dst = &bitmap.pix(y);
		uint8_t *const pdst = &screen.priority().pix(y);


		for (int x = cliprect.left(); x <= cliprect.right(); x++)
		{
			int const realx = (x + scrollx) & 0x1ff;
			uint16_t srcdat = src[realx];

			/* 0x01 - data bits
			   0x02
			   0x04
			   0x08
			   0x10 - extra colour bit? (first boss)
			   0x20 - priority over sprites
			   the old driver also had 'bg_palbase' but I don't see what it's for?
			*/

			if ((srcdat & 0x20) == pri)
			{
				if (srcdat & 0x10) srcdat += 0x400;
				//if (srcdat & 0x10) srcdat += machine().rand()&0x1f;

				srcdat = (srcdat & 0x000f) | ((srcdat & 0xffc0) >> 2);
				dst[x] = srcdat;
				pdst[x] |= pri_mask;
			}


		}
	}
}

uint32_t dynduke_state::screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	screen.priority().fill(0, cliprect);
	/* Setup the tilemaps */
	m_fg_layer->set_scrolly(0, ((m_scroll_ram[0x11] & 0x30) << 4) + ((m_scroll_ram[0x12] & 0x7f) << 1) + ((m_scroll_ram[0x12] & 0x80) >> 7));
	m_fg_layer->set_scrollx(0, ((m_scroll_ram[0x19] & 0x30) << 4) + ((m_scroll_ram[0x1a] & 0x7f) << 1) + ((m_scroll_ram[0x1a] & 0x80) >> 7));
	m_fg_layer->enable(m_fore_enable);
	m_tx_layer->enable(m_txt_enable);

	draw_background(screen, bitmap, cliprect, 0x00, 1);
	draw_background(screen, bitmap, cliprect, 0x20, 2);
	m_fg_layer->draw(screen, bitmap, cliprect, 0,4);
	m_tx_layer->draw(screen, bitmap, cliprect, 0,8);
	draw_sprites(screen, bitmap, cliprect);

	return 0;
}
