#ifndef __STATUS_HPP__
#define __STATUS_HPP__

enum class status
{
	success,
	cli_handler_not_found,
	loading_pump_not_active,
	unloading_pump_not_active,
	storage_tank_non_working,
	low_level_of_oil_products,
	high_level_of_oil_products,
	incorrect_tank_id,
	failed_initialization,
	read_error,
	write_error,
	no_tanks,
	failed_accepted,
	disconnect,
};

namespace st
{
	[[nodiscard]] bool is_success(status st)
	{
		return st == status::success;
	}

	[[nodiscard]] bool is_not_success(status st)
	{
		return !is_success(st);
	}
}

#endif // !__STATUS_HPP__
