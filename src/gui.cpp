#pragma once
#include "common.hpp"
#include "gui.hpp"

#include "natives.hpp"
#include "renderer.hpp"
#include "script.hpp"
#include "notifications/view_notifications.hpp"

#ifdef ENABLE_ASI_LOADER
	#include "asi_loader/script_manager.hpp"
#endif // ENABLE_ASI_LOADER

#include "xml_loader/xml_vehicles_service.hpp"
#include "xml_loader/xml_map_service.hpp"

#include <imgui.h>

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

namespace big
{
	gui::gui() :
	    m_is_open(false),
	    m_override_mouse(false)
	{
		g_renderer->add_dx_callback(view::notifications, -2);
		g_renderer->add_dx_callback(
		    [this] {
			    dx_on_tick();
		    },
		    -5);

#ifdef ENABLE_ASI_LOADER
		g_renderer->add_wndproc_callback([this](HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
			script_manager::wndproc(hwnd, msg, wparam, lparam);
		});
#endif // ENABLE_ASI_LOADER

		dx_init();

		g_gui = this;
	}

	gui::~gui()
	{
		g_gui = nullptr;
	}

	bool gui::is_open()
	{
		return m_is_open;
	}

	void gui::toggle(bool toggle)
	{
		m_is_open = toggle;
	}

	void gui::override_mouse(bool override)
	{
		m_override_mouse = override;
	}

	void gui::dx_init()
	{

	}

	void gui::dx_on_tick()
	{

	}

	void gui::script_on_tick()
	{

	}

	void gui::script_func()
	{
		g_xml_vehicles_service->fetch_xml_files();
		g_xml_map_service->fetch_xml_files();

		LOG(INFO) << PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());

		while (true)
		{
			g_gui->script_on_tick();
			script::get_current()->yield();
		}
	}
}