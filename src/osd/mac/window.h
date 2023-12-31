// license:BSD-3-Clause
// copyright-holders:Olivier Galibert, R. Belmont
//============================================================
//
//  window.h - SDL window handling
//
//  Mac OSD by R. Belmont
//
//============================================================

#ifndef MAME_OSD_MAC_WINDOW_H
#define MAME_OSD_MAC_WINDOW_H

#pragma once

#include "osdmac.h"
#include "osdsync.h"

#include "modules/osdwindow.h"

#include <cstdint>
#include <memory>
#include <list>

//============================================================
//  TYPE DEFINITIONS
//============================================================

class render_target;

typedef uintptr_t HashT;

#define OSDWORK_CALLBACK(name)  void *name(void *param, int threadid)

class mac_window_info : public osd_window_t<void *>
{
public:
	mac_window_info(running_machine &a_machine, render_module &renderprovider, int index, std::shared_ptr<osd_monitor_info> a_monitor,
			const osd_window_config *config);

	~mac_window_info();

	int window_init();

	void update() override;
	void toggle_full_screen();
	void modify_prescale(int dir);
	void resize(int32_t width, int32_t height);
	void complete_destroy() override;

	void capture_pointer() override;
	void release_pointer() override;
	void show_pointer() override;
	void hide_pointer() override;

	void notify_changed();

	osd_dim get_size() override;

	int xy_to_render_target(int x, int y, int *xt, int *yt);

private:
	// window handle and info
	int                 m_startmaximized;

	// dimensions
	osd_dim             m_minimum_dim;
	osd_dim             m_windowed_dim;

	// rendering info
	osd_event           m_rendered_event;

	//int                 m_extra_flags;

	// returns 0 on success, else 1
	int complete_create();

private:
	int wnd_extra_width();
	int wnd_extra_height();
	osd_rect constrain_to_aspect_ratio(const osd_rect &rect, int adjustment);
	osd_dim get_min_bounds(int constrain);
	osd_dim get_max_bounds(int constrain);
	void update_cursor_state();
	osd_dim pick_best_mode();
	void set_fullscreen(int afullscreen) { m_fullscreen = afullscreen; }

	// monitor info
	bool                               m_mouse_captured;
	bool                               m_mouse_hidden;

	void measure_fps(int update);
};

#endif // MAME_OSD_MAC_WINDOW_H
