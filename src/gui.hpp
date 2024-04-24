#pragma once
#include "common.hpp"

namespace big
{
	class gui
	{
	public:
		gui();
		virtual ~gui();
		gui(const gui&)                = delete;
		gui(gui&&) noexcept            = delete;
		gui& operator=(const gui&)     = delete;
		gui& operator=(gui&&) noexcept = delete;

		bool is_open();
		void toggle(bool toggle);

		bool mouse_override() const
		{
			return m_override_mouse;
		}
		/**
		 * @brief Forces the mouse to draw and disable camera controls of the game
		 * This function works for now but might start causing issues when more features start relying on it.
		 */
		void override_mouse(bool override);

		void dx_init();
		/**
		* @brief Used to draw the GUI.
		*/
		void dx_on_tick();

		/**
		* @brief Disables controll when gui is open.
		*/
		void script_on_tick();
		static void script_func();

	private:
		bool m_is_open;
		bool m_override_mouse;
	};

	inline gui* g_gui;
}
