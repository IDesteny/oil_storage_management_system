#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__

#include <iostream>
#include <vector>
#include <sstream>
#include <memory>

#include "dye.hpp"
#include "message_connection.hpp"

class client
{
private:
	int tank_id;
	
	template <typename T>
	[[nodiscard]] std::pair<std::string, status> get_request(std::shared_ptr<T> connection, std::string_view request) const
	{
		if (auto result = connection->write(request); st::is_not_success(result))
		{
			return { std::string(), result };
		}
		
		auto response = std::string();
		if (auto result = connection->read(response); st::is_not_success(result))
		{
			return { std::string(), result };
		}
		
		return { response, status::success };
	}
	
	template <typename T>
	[[nodiscard]] std::pair<std::string, status> get_complete_info(std::shared_ptr<T> connection) const
	{
		auto &&[working_state, result_work_state] = get_request(connection, "get working state");
		if (st::is_not_success(result_work_state))
		{
			return { working_state, result_work_state };
		}

		auto &&[loading_pump_status, result_loading_pump_status] = get_request(connection, "get loading pump status");
		if (st::is_not_success(result_loading_pump_status))
		{
			return { loading_pump_status, result_loading_pump_status };
		}
		
		auto &&[unloading_pump_status, result_unloading_pump_status] = get_request(connection, "get unloading pump status");
		if (st::is_not_success(result_unloading_pump_status))
		{
			return { unloading_pump_status, result_unloading_pump_status };
		}
		
		auto &&[lower_permissible_level, result_lower_permissible_level] = get_request(connection, "get lower permissible level");
		if (st::is_not_success(result_lower_permissible_level))
		{
			return { lower_permissible_level, result_lower_permissible_level };
		}
		
		auto &&[upper_acceptable_level, result_upper_acceptable_level] = get_request(connection, "get upper acceptable level");
		if (st::is_not_success(result_upper_acceptable_level))
		{
			return { upper_acceptable_level, result_upper_acceptable_level };
		}
		
		auto &&[download_speed, result_download_speed] = get_request(connection, "get download speed");
		if (st::is_not_success(result_download_speed))
		{
			return { download_speed, result_download_speed };
		}
		
		auto &&[unloading_speed, result_unloading_speed] = get_request(connection, "get unloading speed");
		if (st::is_not_success(result_unloading_speed))
		{
			return { unloading_speed, result_unloading_speed };
		}
		
		auto &&[level_of_oil_products, result_level_of_oil_products] = get_request(connection, "get level of oil products");
		if (st::is_not_success(result_level_of_oil_products))
		{
			return { level_of_oil_products, result_level_of_oil_products };
		}
		
		static const auto max_level = 6;
		auto terminal_dye = dye(dye::code::white);
		auto complete_info = std::stringstream();
		auto quantity_of_oil_products = (std::stoull(level_of_oil_products) * max_level) / std::stoull(upper_acceptable_level);
		
		complete_info << "     -= Oil storage =-\n\n";
		
		if (working_state == "non-work")
		{
			complete_info << terminal_dye.forever(dye::code::gray);
		}
		
		for (uint64_t i = 0; i < max_level; ++i)
		{
			if (i < max_level - quantity_of_oil_products)
			{
				if (i == max_level - 1)
				{
					complete_info << "       " << terminal_dye.colorant(">", unloading_pump_status == "active" ? dye::code::green : dye::code::red)
								<< "|         |" << terminal_dye.colorant(">", loading_pump_status == "active" ? dye::code::green : dye::code::red) << '\n';
				}
				else
				{
					complete_info << "\t|         |\n";
				}
			}
			else
			{
				if (i == max_level - 1)
				{
					complete_info << "       " << terminal_dye.colorant(">", unloading_pump_status == "active" ? dye::code::green : dye::code::red)
										<< '|' << terminal_dye.colorant("#########", quantity_of_oil_products == 1 ? dye::code::red : dye::code::yellow)
										<< '|' << terminal_dye.colorant(">", loading_pump_status == "active" ? dye::code::green : dye::code::red) << '\n';
				}
				else
				{
					complete_info << "\t|" << terminal_dye.colorant("#########", dye::code::yellow) << "|\n";
				}
			}
		}
		
		complete_info << "\t\\=========/\n\n";
		
		if (working_state == "non-work")
		{
			complete_info << terminal_dye.forever(dye::code::white);
		}
		
		complete_info << "working state..............." << terminal_dye.colorant(working_state, working_state == "work" ? dye::code::green : dye::code::red) << '\n';
		complete_info << "loading pump status........." << terminal_dye.colorant(loading_pump_status, loading_pump_status == "active" ? dye::code::green : dye::code::red) << '\n';
		complete_info << "unloading pump status......." << terminal_dye.colorant(unloading_pump_status, unloading_pump_status == "active" ? dye::code::green : dye::code::red) << '\n';
		complete_info << "lower permissible level....." << lower_permissible_level << '\n';
		complete_info << "upper acceptable level......" << upper_acceptable_level << '\n';
		complete_info << "download speed.............." << download_speed << '\n';
		complete_info << "unloading speed............." << unloading_speed << '\n';
		complete_info << "level of oil products......." << terminal_dye.colorant(level_of_oil_products, level_of_oil_products == lower_permissible_level
													|| level_of_oil_products == upper_acceptable_level ? dye::code::red : dye::code::white) << '\n';
		
		return { complete_info.str(), status::success };
	}
	
public:	
	explicit client(int tank_id) : tank_id(tank_id)
	{}

	template <typename T>
	[[nodiscard]] std::pair<std::shared_ptr<T>, status> connect()
	{
		auto result = status::success;
		if (auto handshake_connection = std::make_shared<T>(result); st::is_success(result))
		{
			if (result = handshake_connection->write(std::to_string(tank_id)); st::is_not_success(result))
			{
				return { nullptr, result };
			}
			
			auto connection_key = std::string();
			if (result = handshake_connection->read(connection_key); st::is_not_success(result))
			{
				return { nullptr, result };
			}
			
			if (auto session_connection = std::make_shared<T>(result, std::stoi(connection_key)); st::is_success(result))
			{
				std::cout << "the oil tank is busy, please wait...\n";
				
				auto acceptance_message = std::string();
				if (result = session_connection->read(acceptance_message); st::is_not_success(result))
				{
					return { nullptr, result };
				}
				
				if (acceptance_message != "-- accepted --")
				{
					return { nullptr, status::failed_accepted };
				}
				
				return { session_connection, status::success };
			}
		}
		return { nullptr, result };
	}
	
	status run()
	{
		auto &&[connection, connection_result] = connect<message_connection>();
		if (st::is_not_success(connection_result))
		{
			return connection_result;
		}
		
		// cppcheck-suppress cppcheckError
		if (auto &&[complete_info, complete_info_result] = get_complete_info(connection); st::is_success(complete_info_result))
		{
			system("clear");
			std::cout << complete_info << std::endl;
		}
		else return complete_info_result;
		
		while (true)
		{
			std::cout << "> ";
			
			auto user_command = std::string();
			std::getline(std::cin, user_command);
			
			if (auto result = connection->write(user_command); st::is_not_success(result))
			{
				return result;
			}
			
			if (user_command == "disconnect")
			{
				return status::success;
			}
			
			std::cout << "wait for the command...\n";
			
			auto server_response = std::string();
			if (auto result = connection->read(server_response); st::is_not_success(result))
			{
				return result;
			}
			
			if (auto &&[complete_info, complete_info_result] = get_complete_info(connection); st::is_success(complete_info_result))
			{
				system("clear");
				std::cout << complete_info << std::endl;
				std::cout << "% " << server_response << " %" << std::endl;
			}
			else return complete_info_result;
		}
	}
};


#endif // !__CLIENT_HPP__
