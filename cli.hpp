#ifndef __CLI_HPP__
#define __CLI_HPP__

#include <regex>

#include "storage_tank.hpp"
#include "connection_if.hpp"

using session_t = std::pair<std::shared_ptr<connection_if>, storage_tank &>;

class cli
{
private:
	static inline std::vector<std::pair<std::regex, std::function<status(std::smatch &, session_t &)>>> cli_handler
	{
		{ std::regex("set download speed (\\d+)"),
			[](std::smatch &sm, session_t &session)
			{
				auto &&[current_session, current_tank] = session;
				current_tank.set_download_speed(std::stoull(sm[1]));
				return current_session->write("success");
			}
		},
		{ std::regex("set unloading speed (\\d+)"),
			[](std::smatch &sm, session_t &session)
			{
				auto &&[current_session, current_tank] = session;
				current_tank.set_unloading_speed(std::stoull(sm[1]));
				return current_session->write("success");
			}
		},
		{ std::regex("set lower permissible level (\\d+)"),
			[](std::smatch &sm, session_t &session)
			{
				auto &&[current_session, current_tank] = session;
				current_tank.set_lower_permissible_level(std::stoull(sm[1]));
				return current_session->write("success");
			}
		},
		{ std::regex("set upper acceptable level (\\d+)"),
			[](std::smatch &sm, session_t &session)
			{
				auto &&[current_session, current_tank] = session;
				current_tank.set_upper_acceptable_level(std::stoull(sm[1]));
				return current_session->write("success");
			}
		},
		{ std::regex("set level of oil products (\\d+)"),
			[](std::smatch &sm, session_t &session)
			{
				auto &&[current_session, current_tank] = session;
				current_tank.set_level_of_oil_products(std::stoull(sm[1]));
				return current_session->write("success");
			}
		},
		{ std::regex("set working state (work|non-work)"),
			[](std::smatch &sm, session_t &session)
			{
				auto &&[current_session, current_tank] = session;
				current_tank.set_working_state(st::stows(sm[1]));
				return current_session->write("success");
			}
		},
		{ std::regex("set loading pump status (active|inactive)"),
			[](std::smatch &sm, session_t &session)
			{
				auto &&[current_session, current_tank] = session;
				current_tank.set_loading_pump_status(st::stoas(sm[1]));
				return current_session->write("success");
			}
		},
		{ std::regex("set unloading pump status (active|inactive)"),
			[](std::smatch &sm, session_t &session)
			{
				auto &&[current_session, current_tank] = session;
				current_tank.set_unloading_pump_status(st::stoas(sm[1]));
				return current_session->write("success");
			}
		},
		{ std::regex("get download speed"),
			[](std::smatch &sm, session_t &session)
			{
				auto &&[current_session, current_tank] = session;
				return current_session->write(std::to_string(current_tank.get_download_speed()));
			}
		},
		{ std::regex("get unloading speed"),
			[](std::smatch &sm, session_t &session)
			{
				auto &&[current_session, current_tank] = session;
				return current_session->write(std::to_string(current_tank.get_unloading_speed()));
			}
		},
		{ std::regex("get lower permissible level"),
			[](std::smatch &sm, session_t &session)
			{
				auto &&[current_session, current_tank] = session;
				return current_session->write(std::to_string(current_tank.get_lower_permissible_level()));
			}
		},
		{ std::regex("get upper acceptable level"),
			[](std::smatch &sm, session_t &session)
			{
				auto &&[current_session, current_tank] = session;
				return current_session->write(std::to_string(current_tank.get_upper_acceptable_level()));
			}
		},
		{ std::regex("get level of oil products"),
			[](std::smatch &sm, session_t &session)
			{
				auto &&[current_session, current_tank] = session;
				return current_session->write(std::to_string(current_tank.get_level_of_oil_products()));
			}
		},
		{ std::regex("get working state"),
			[](std::smatch &sm, session_t &session)
			{
				auto &&[current_session, current_tank] = session;
				return current_session->write(st::wstos(current_tank.get_working_state()));
			}
		},
		{ std::regex("get loading pump status"),
			[](std::smatch &sm, session_t &session)
			{
				auto &&[current_session, current_tank] = session;
				return current_session->write(st::astos(current_tank.get_loading_pump_status()));
			}
		},
		{ std::regex("get unloading pump status"),
			[](std::smatch &sm, session_t &session)
			{
				auto &&[current_session, current_tank] = session;
				return current_session->write(st::astos(current_tank.get_unloading_pump_status()));
			}
		},
		{ std::regex("download (\\d+)"),
			[](std::smatch &sm, session_t &session)
			{
				auto &&[current_session, current_tank] = session;
				auto oil = oil_product(std::stoull(sm[1]));
				
				if (auto result = current_tank.download(oil); st::is_not_success(result))
				{
					return result;
				}
				
				return current_session->write("success");
			}
		},
		{ std::regex("unload (\\d+)"),
			[](std::smatch &sm, session_t &session)
			{
				auto &&[current_session, current_tank] = session;
				auto oil = oil_product(std::stoull(sm[1]));
				oil.set_content_volume(oil.get_capacity()); // TODO
								
				if (auto result = current_tank.unload(oil); st::is_not_success(result))
				{
					return result;
				}
				
				return current_session->write("success");
			}
		},
		{ std::regex("help"),
			[](std::smatch &sm, session_t &session)
			{
				auto &&[current_session, current_tank] = session;
				auto help_info = std::stringstream();
				
				help_info << "set download speed <number>\n"
					<< "set unloading speed <number>\n"
					<< "set lower permissible level <number>\n"
					<< "set upper acceptable level <number>\n"
					<< "set level of oil product <number>\n"
					<< "set working state <work|non-work>\n"
					<< "set loading pump status <active|inactive>\n"
					<< "set unloading pump status <active|inactive>\n"
					<< "get download speed\n"
					<< "get unloading speed\n"
					<< "get lower permissible level\n"
					<< "get upper acceptable level\n"
					<< "get level of oil product\n"
					<< "get working state\n"
					<< "get loading pump status\n"
					<< "get unloading pump status\n"
					<< "download <quantity of oil products (number)>\n"
					<< "unload <quantity of oil products (number)>\n"
					<< "help\n"
					<< "disconnect";
					
				return current_session->write(help_info.str());
			}
		},
		{ std::regex("disconnect"),
			[](std::smatch &sm, session_t &session)
			{
				return status::disconnect;
			}
		},
	};

public:
	static status handling(const std::string &command, session_t &session)
	{
		for (auto &&[regexp, handler] : cli_handler)
		{
			std::smatch matched;
			if (std::regex_search(command, matched, regexp))
			{
				return handler(matched, session);
			}
		}

		return status::cli_handler_not_found;
	}
};

#endif // !__CLI_HPP__
