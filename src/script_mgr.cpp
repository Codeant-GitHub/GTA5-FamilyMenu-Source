#pragma once
#include "common.hpp"
#include "script_mgr.hpp"

#include "gta/script_thread.hpp"
#include "gta_util.hpp"
#include "invoker.hpp"
#include "pointers.hpp"
#include "script/tlsContext.hpp"

namespace big
{
	void script_mgr::add_script(std::unique_ptr<script> script)
	{
		std::lock_guard lock(m_mutex);

		m_scripts.push_back(std::move(script));
	}

	void script_mgr::remove_all_scripts()
	{
		std::lock_guard lock(m_mutex);

		m_scripts.clear();
	}


	void script_mgr::ensure_main_fiber()
	{
		ConvertThreadToFiber(nullptr);

		m_can_tick = true;
	}

	void script_mgr::tick()
	{
		static bool ensure_it = (ensure_main_fiber(), true);

		std::lock_guard lock(m_mutex);

		for (const auto& script : m_scripts)
		{
			if (script->is_enabled())
				script->tick();
		}
	}
}
