#pragma once
#include "gta/pools.hpp"

namespace big::pools
{
	inline auto& get_all_peds()
	{
		return **g_pointers->m_gta.m_ped_pool;
	}

	inline auto& get_all_vehicles()
	{
		return ***g_pointers->m_gta.m_vehicle_pool;
	}

	inline auto& get_all_props()
	{
		return **g_pointers->m_gta.m_prop_pool;
	}

	inline auto& get_all_pickups()
	{
		return **g_pointers->m_gta.m_pickup_pool;
	}

#ifdef ENABLE_ASI_LOADER
	inline auto& get_all_cameras()
	{
		return **g_pointers->m_gta.m_camera_pool;
	}
#endif

	inline auto get_all_peds_array()
	{
		return get_all_peds().to_array();
	}

	inline auto get_all_vehicles_array()
	{
		return get_all_vehicles().to_array();
	}

	inline auto get_all_props_array()
	{
		return get_all_props().to_array();
	}
	
	inline auto get_all_pickups_array()
	{
		return get_all_pickups().to_array();
	}

#ifdef ENABLE_ASI_LOADER	
	inline auto get_all_cameras_array()
	{
		return get_all_cameras().to_array();
	}
#endif
};