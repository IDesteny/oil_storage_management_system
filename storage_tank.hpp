#ifndef __STORAGE_TANK_HPP__
#define __STORAGE_TANK_HPP__

#include "status.hpp"
#include "logging.hpp"
#include "oil_product.hpp"

enum class working_state
{
	work,
	non_work
};

enum class activity_state
{
	active,
	inactive
};

namespace st
{
	[[nodiscard]] working_state stows(const std::string &state)
	{
		return state == "work" ? working_state::work : working_state::non_work;
	}

	[[nodiscard]] activity_state stoas(const std::string &state)
	{
		return state == "active" ? activity_state::active : activity_state::inactive;
	}

	[[nodiscard]] std::string wstos(working_state state)
	{
		return state == working_state::work ? "work" : "non-work";
	}

	[[nodiscard]] std::string astos(activity_state state)
	{
		return state == activity_state::active ? "active" : "inactive";
	}
};

class storage_tank
{
private:
	working_state work_state = working_state::non_work;

	activity_state loading_pump_status = activity_state::inactive;
	activity_state unloading_pump_status = activity_state::inactive;

	uint64_t lower_permissible_level = 10;
	uint64_t upper_acceptable_level = 1000;

	uint64_t download_speed = 100;
	uint64_t unloading_speed = 100;

	uint64_t level_of_oil_products = lower_permissible_level;

	std::mutex _mutex;

public:
	void set_download_speed(uint64_t speed) noexcept
	{
		download_speed = speed;
	}

	void set_unloading_speed(uint64_t speed) noexcept
	{
		unloading_speed = speed;
	}

	void set_lower_permissible_level(uint64_t level) noexcept
	{
		lower_permissible_level = level;
	}

	void set_upper_acceptable_level(uint64_t level) noexcept
	{
		upper_acceptable_level = level;
	}

	void set_level_of_oil_products(uint64_t level) noexcept
	{
		level_of_oil_products = level;
	}
	
	void set_working_state(working_state state) noexcept
	{
		work_state = state;
	}

	void set_loading_pump_status(activity_state status) noexcept
	{
		loading_pump_status = status;
	}

	void set_unloading_pump_status(activity_state status) noexcept
	{
		unloading_pump_status = status;
	}

	[[nodiscard]] uint64_t get_download_speed() const noexcept
	{
		return download_speed;
	}

	[[nodiscard]] uint64_t get_unloading_speed() const noexcept
	{
		return unloading_speed;
	}

	[[nodiscard]] uint64_t get_lower_permissible_level() const noexcept
	{
		return lower_permissible_level;
	}

	[[nodiscard]] uint64_t get_upper_acceptable_level() const noexcept
	{
		return upper_acceptable_level;
	}

	[[nodiscard]] uint64_t get_level_of_oil_products() const noexcept
	{
		return level_of_oil_products;
	}
	
	[[nodiscard]] working_state get_working_state() const noexcept
	{
		return work_state;
	}

	[[nodiscard]] activity_state get_loading_pump_status() const noexcept
	{
		return loading_pump_status;
	}

	[[nodiscard]] activity_state get_unloading_pump_status() const noexcept
	{
		return unloading_pump_status;
	}

	[[nodiscard]] std::mutex &_get_sync_object()
	{
		return _mutex;
	}

	status download(oil_product &op)
	{
		if (work_state == working_state::non_work)
		{
			return status::storage_tank_non_working;
		}

		if (loading_pump_status == activity_state::inactive)
		{
			return status::loading_pump_not_active;
		}

		logging::inflog("== download request ==");

		auto required_download_size = op.get_capacity() - op.get_content_volume();
		auto possible_loading_volume = level_of_oil_products - lower_permissible_level;
		auto total_download_volume = std::min(required_download_size, possible_loading_volume);

		logging::inflog("required download size: " + std::to_string(required_download_size));
		logging::inflog("possible loading volume: " + std::to_string(possible_loading_volume));
		logging::inflog("total download volume: " + std::to_string(total_download_volume));

		if (total_download_volume == 0)
		{
			return status::low_level_of_oil_products;
		}

		auto loading_time = total_download_volume / download_speed;

		logging::inflog("loading time: ");

		// Simulation of system operation
		std::this_thread::sleep_for(std::chrono::seconds(loading_time));

		level_of_oil_products -= total_download_volume;

		logging::inflog("level of oil products: ");

		if (level_of_oil_products == lower_permissible_level)
		{
			loading_pump_status = activity_state::inactive;

			logging::inflog("load pump inactive");
		}

		/*if (unloading_pump_status == activity_state::inactive &&
			level_of_oil_products < upper_acceptable_level)
		{
			unloading_pump_status = activity_state::active;

			logging::inflog("unloading pump active");
		}*/

		op.set_content_volume(op.get_content_volume() + total_download_volume);

		return status::success;
	}

	status unload(oil_product &op)
	{
		if (work_state == working_state::non_work)
		{
			return status::storage_tank_non_working;
		}

		if (unloading_pump_status == activity_state::inactive)
		{
			return status::unloading_pump_not_active;
		}

		logging::inflog("== unload request ==");

		auto possible_unloading_size = op.get_content_volume();
		auto possible_unloading_volume = upper_acceptable_level - level_of_oil_products;
		auto total_unloading_volume = std::min(op.get_content_volume(), possible_unloading_volume);

		logging::inflog("possible unloading size: " + std::to_string(possible_unloading_size));
		logging::inflog("possible unloading volume: " + std::to_string(possible_unloading_volume));
		logging::inflog("total unloading volume: " + std::to_string(total_unloading_volume));

		if (total_unloading_volume == 0)
		{
			return status::high_level_of_oil_products;
		}

		auto unloading_time = total_unloading_volume / unloading_speed;

		logging::inflog("unloading time: " + std::to_string(unloading_time));

		// Simulation of system operation
		std::this_thread::sleep_for(std::chrono::seconds(unloading_time));

		level_of_oil_products += total_unloading_volume;

		logging::inflog("level of oil products: " + std::to_string(level_of_oil_products));

		if (level_of_oil_products == upper_acceptable_level)
		{
			unloading_pump_status = activity_state::inactive;

			logging::inflog("unloading pump inactive");
		}

		/*if (loading_pump_status == activity_state::inactive &&
			level_of_oil_products > lower_permissible_level)
		{
			loading_pump_status = activity_state::active;

			logging::inflog("load pump active");
		}*/

		op.set_content_volume(op.get_content_volume() - total_unloading_volume);

		return status::success;
	}
};

#endif // !__STORAGE_TANK_HPP__
