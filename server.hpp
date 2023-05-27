#ifndef __SERVER_HPP__
#define __SERVER_HPP__
#define _IS_SERVER_

#include <thread>
#include <mutex>
#include <random>

#include "cli.hpp"
#include "message_connection.hpp"

class server
{
private:
	std::vector<storage_tank> storage_tanks;

public:
	explicit server(size_t number_of_tanks): storage_tanks{ number_of_tanks }
	{}

	template <class T>
	[[nodiscard]] std::pair<std::optional<session_t>, status> accept()
	{
		auto result = status::success;
		if (static auto accept_connection = std::make_shared<T>(result); st::is_success(result))
		{
			logging::inflog("waiting for connection on key: " + std::to_string(std::numeric_limits<int>::max()));
			
			auto storage_tank_id = std::string();
			if (result = accept_connection->read(storage_tank_id); st::is_success(result))
			{
				logging::inflog("a client with an authorization request has connected to the tank number: " + storage_tank_id);
				
				try
				{
					static auto rand_device = std::random_device();
					auto &required_tank = storage_tanks.at(std::stoull(storage_tank_id));
					auto session_key = std::default_random_engine(rand_device())();
					
					logging::inflog("session key: " + std::to_string(session_key));
					
					if (auto new_session = session_t{ std::make_shared<T>(result, session_key), required_tank }; st::is_success(result))
					{
						if (result = accept_connection->write(std::to_string(session_key)); st::is_success(result))
						{
							return { new_session, status::success };
						}
					}
				}
				catch(...)
				{
					result = status::incorrect_tank_id;
				}
			}
		}
		
		return { std::nullopt, result };
	}

	void connect_handler(session_t &session)
	{
		logging::inflog("waiting for an existing session to be released");
		
		auto client_command = std::string();
		auto &&[current_session, current_tank] = session;
		auto guard = std::lock_guard(current_tank._get_sync_object());
		
		if (auto result = current_session->write("-- accepted --"); st::is_not_success(result))
		{
			logging::errlog("sending a customer acceptance message");
			return;
		}
		
		logging::inflog("session permission message sent");
		
		while (true)
		{
			logging::inflog("waiting for client command");
			
			if (auto result = current_session->read(client_command); st::is_not_success(result))
			{
				logging::errlog("receiving a command from the client");
				break;
			}

			logging::inflog("command processing: " + client_command);
			
			switch (auto result_handling = cli::handling(client_command, session))
			{
				case status::success:
				{
					break;
				}
				
				case status::cli_handler_not_found:
				{
					logging::warnlog("no handler found for client command");
					
					if (auto result = current_session->write("unknow command"); st::is_not_success(result))
					{
						logging::errlog("write error");
						return;
					}
					break;
				}
				
				case status::loading_pump_not_active:
				{
					logging::warnlog("it is not possible to unload, the corresponding pump is inactive");
					
					if (auto result = current_session->write("loading pump not active"); st::is_not_success(result))
					{
						logging::errlog("write error");
						return;
					}
					break;
				}
				
				case status::unloading_pump_not_active:
				{
					logging::warnlog("unable to load, the corresponding pump is inactive");
					
					if (auto result = current_session->write("unloading pump not active"); st::is_not_success(result))
					{
						logging::errlog("write error");
						return;
					}
					break;
				}
				
				case status::storage_tank_non_working:
				{
					logging::warnlog("storage tank non working");
					
					if (auto result = current_session->write("oil tank not working"); st::is_not_success(result))
					{
						logging::errlog("write error");
						return;
					}
					break;
				}
				
				case status::low_level_of_oil_products:
				{
					logging::warnlog("critically low level of oil products");
					
					if (auto result = current_session->write("too low level of oil in the tank, it is impossible to download"); st::is_not_success(result))
					{
						logging::errlog("write error");
						return;
					}
					break;
				}
				
				case status::high_level_of_oil_products:
				{
					logging::warnlog("critically high level of oil products");
					
					if (auto result = current_session->write("too high level of oil in the tank, it is impossible to unload"); st::is_not_success(result))
					{
						logging::errlog("write error");
						return;
					}
					break;
				}
				
				case status::disconnect:
				{
					logging::inflog("client disconnected");
					return;
				}
				
				default:
				{
					logging::warnlog("unhandled error: " + std::to_string((int)result_handling));
					break;
				}
			}
		}
	}

	status run()
	{
		while (true)
		{
			// cppcheck-suppress cppcheckError
			if (auto &&[session, result] = accept<message_connection>(); st::is_success(result))
			{
				auto bind_connect_handler = std::bind(&server::connect_handler, this, session.value());
				auto thread_handler = std::thread(bind_connect_handler);

				thread_handler.detach();
			}
			else return result;
		}
	}
};

#endif // !__SERVER_HPP__
