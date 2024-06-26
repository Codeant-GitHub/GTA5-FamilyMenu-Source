#ifndef COMMON_INC
#define COMMON_INC

// clang-format off

#include <sdkddkver.h>
#include <winsock2.h>
#include <windows.h>
#include <d3d11.h>

#include <cinttypes>
#include <cstddef>
#include <cstdint>

#include <chrono>
#include <ctime>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <iomanip>

#include <atomic>
#include <mutex>
#include <thread>

#include <memory>
#include <new>

#include <sstream>
#include <string>
#include <string_view>

#include <algorithm>
#include <functional>
#include <utility>

#include <set>
#include <unordered_set>
#include <stack>
#include <vector>

#include <typeinfo>
#include <type_traits>

#include <exception>
#include <stdexcept>

#include <any>
#include <optional>
#include <variant>

#include <format>
#include <nlohmann/json.hpp>

#include "logger/logger.hpp"

#include "gta/natives.hpp"
#include "ped/CPed.hpp"

#include "notifications/notification_service.hpp"

// clang-format on

namespace big
{
	using namespace std::chrono_literals;

	inline HMODULE g_hmodule{};
	inline HANDLE g_main_thread{};
	inline DWORD g_main_thread_id{};
	inline std::atomic_bool g_running{false};

	inline CPed* g_local_player;
}

namespace self
{
	inline Ped ped;
	inline Player id;
	inline Vector3 pos;
	inline Vector3 rot;
	inline Vehicle veh;
	inline int char_index;
	inline std::unordered_set<int> spawned_vehicles;
}

namespace settings
{
	inline int g_m_remote_controller_vehicle = -1;
	inline int g_m_remote_controlled_vehicle = -1;
}

template<size_t N>
struct template_str
{
	constexpr template_str(const char (&str)[N])
	{
		std::copy_n(str, N, value);
	}

	char value[N];
};

#endif
