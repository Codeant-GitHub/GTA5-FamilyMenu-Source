#pragma once
#include "common.hpp"
#include "byte_patch_manager.hpp"
#include "fiber_pool.hpp"
#include "gui.hpp"
#include "hooking.hpp"
#include "logger/exception_handler.hpp"
#include "pointers.hpp"
#include "rage/gameSkeleton.hpp"
#include "renderer.hpp"
#include "script_mgr.hpp"

#ifdef ENABLE_ASI_LOADER
#include "asi_loader/asi_scripts.hpp"
#include "shv_runner.hpp"
#endif // ENABLE_ASI_LOADER

#include "notifications/notification_service.hpp"
#include "xml_loader/xml_vehicles_service.hpp"
#include "xml_loader/xml_map_service.hpp"
#include "thread_pool.hpp"

namespace big
{
	bool disable_anticheat_skeleton()
	{
		bool patched = false;
		for (rage::game_skeleton_update_mode* mode = g_pointers->m_gta.m_game_skeleton->m_update_modes; mode; mode = mode->m_next)
		{
			for (rage::game_skeleton_update_base* update_node = mode->m_head; update_node; update_node = update_node->m_next)
			{
				if (update_node->m_hash != RAGE_JOAAT("Common Main"))
					continue;
				rage::game_skeleton_update_group* group = reinterpret_cast<rage::game_skeleton_update_group*>(update_node);
				for (rage::game_skeleton_update_base* group_child_node = group->m_head; group_child_node;
					group_child_node = group_child_node->m_next)
				{
					// TamperActions is a leftover from the old AC, but still useful to block anyway
					if (group_child_node->m_hash != 0xA0F39FB6 && group_child_node->m_hash != RAGE_JOAAT("TamperActions"))
						continue;
					patched = true;
					//LOG(INFO) << "Patching problematic skeleton update";
					reinterpret_cast<rage::game_skeleton_update_element*>(group_child_node)->m_function =
						g_pointers->m_gta.m_nullsub;
				}
				break;
			}
		}

		for (rage::skeleton_data& i : g_pointers->m_gta.m_game_skeleton->m_sys_data)
		{
			if (i.m_hash != 0xA0F39FB6 && i.m_hash != RAGE_JOAAT("TamperActions"))
				continue;
			i.m_init_func = reinterpret_cast<uint64_t>(g_pointers->m_gta.m_nullsub);
			i.m_shutdown_func = reinterpret_cast<uint64_t>(g_pointers->m_gta.m_nullsub);
		}
		return patched;
	}
}

BOOL APIENTRY DllMain(HMODULE hmod, DWORD reason, PVOID)
{
	using namespace big;
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hmod);
		g_hmodule = hmod;
		g_main_thread = CreateThread(
			nullptr,
			0,
			[](PVOID) -> DWORD {
				auto handler = exception_handler();

				while (!FindWindow("grcWindow", nullptr))
					std::this_thread::sleep_for(100ms);
				std::filesystem::path base_dir = std::getenv("appdata");
				base_dir /= "FamilyGTA";
				g_file_manager.init(base_dir);

				auto logger_instance = std::make_unique<logger>("FamilyGTA", g_file_manager.get_project_file("./cout.log"));

				EnableMenuItem(GetSystemMenu(GetConsoleWindow(), 0), SC_CLOSE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);

				std::srand(std::chrono::system_clock::now().time_since_epoch().count());

				LOG(INFO) << "FamilyMenu Initializing";

				auto thread_pool_instance = std::make_unique<thread_pool>();
				LOG(INFO) << "Thread pool initialized.";

				auto pointers_instance = std::make_unique<pointers>();
				LOG(INFO) << "Pointers initialized.";

				while (!disable_anticheat_skeleton())
				{
					LOG(WARNING) << "Failed patching anticheat gameskeleton (injected too early?). Waiting 500ms and trying again";
					std::this_thread::sleep_for(500ms);
				}
				LOG(INFO) << "Disabled anticheat gameskeleton.";

				auto byte_patch_manager_instance = std::make_unique<byte_patch_manager>();
				LOG(INFO) << "Byte Patch Manager initialized.";

				auto renderer_instance = std::make_unique<renderer>();
				LOG(INFO) << "Renderer initialized.";
				auto gui_instance = std::make_unique<gui>();

				auto fiber_pool_instance = std::make_unique<fiber_pool>(11);
				LOG(INFO) << "Fiber pool initialized.";

				auto hooking_instance = std::make_unique<hooking>();
				LOG(INFO) << "Hooking initialized.";

				auto notification_service_instance = std::make_unique<notification_service>();
				auto xml_vehicles_service_instance = std::make_unique<xml_vehicles_service>();
				auto xml_maps_service_instance = std::make_unique<xml_map_service>();
				LOG(INFO) << "Registered service instances...";

				g_script_mgr.add_script(std::make_unique<script>(&gui::script_func, "GUI", false));
#ifdef ENABLE_ASI_LOADER
				g_script_mgr.add_script(std::make_unique<script>(&shv_runner::script_func));
#endif // ENABLE_ASI_LOADER

				LOG(INFO) << "Scripts registered.";

				g_hooking->enable();
				LOG(INFO) << "Hooking enabled.";

#ifdef ENABLE_ASI_LOADER
				asi_loader::initialize();
				LOG(INFO) << "ASI Loader initialized.";
#endif // ENABLE_ASI_LOADER

				g_running = true;

				while (g_running)
				{
					if (GetAsyncKeyState(VK_END) & 0x8000)
					{
						g_running = false;
					}
					std::this_thread::sleep_for(10ms);
				}

#ifdef ENABLE_ASI_LOADER
				shv_runner::shutdown();
				LOG(INFO) << "ASI plugins unloaded.";
#endif // ENABLE_ASI_LOADER
				g_script_mgr.remove_all_scripts();
				LOG(INFO) << "Scripts unregistered.";

				g_hooking->disable();
				LOG(INFO) << "Hooking disabled.";

				// Make sure that all threads created don't have any blocking loops
				// otherwise make sure that they have stopped executing
				thread_pool_instance->destroy();
				LOG(INFO) << "Destroyed thread pool.";

				xml_vehicles_service_instance.reset();
				LOG(INFO) << "Xml Vehicles Service reset.";

				xml_maps_service_instance.reset();
				LOG(INFO) << "Xml Maps Service reset.";

				hooking_instance.reset();
				LOG(INFO) << "Hooking uninitialized.";

				fiber_pool_instance.reset();
				LOG(INFO) << "Fiber pool uninitialized.";

				renderer_instance.reset();
				LOG(INFO) << "Renderer uninitialized.";

				byte_patch_manager_instance.reset();
				LOG(INFO) << "Byte Patch Manager uninitialized.";

				pointers_instance.reset();
				LOG(INFO) << "Pointers uninitialized.";

				thread_pool_instance.reset();
				LOG(INFO) << "Thread pool uninitialized.";

				LOG(INFO) << "Farewell!";
				logger_instance->destroy();
				logger_instance.reset();

				CloseHandle(g_main_thread);
				FreeLibraryAndExitThread(g_hmodule, 0);
			},
			nullptr,
			0,
			&g_main_thread_id);
	}

	return true;
}
