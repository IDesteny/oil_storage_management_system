#include "server.hpp"

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		logging::errlog("you must specify the number of tanks in the arguments");
		return -1;
	}
	
	try
	{
		switch (auto result = server(std::stoull(argv[1])).run())
		{
			case status::failed_initialization:
			{
				logging::errlog("error initializing new session");
				return -1;
			}
			
			case status::incorrect_tank_id:
			{
				logging::errlog("incorrect tank ID");
				return -1;
			}
			
			case status::write_error:
			{
				logging::errlog("error writing message to client");
				return -1;
			}
			
			case status::read_error:
			{
				logging::errlog("error reading message from client");
				return -1;
			}
			
			default:
			{
				logging::warnlog("unhandled error: " + std::to_string((int)result));
				return -1;
			}
		}
	}
	catch (...)
	{
		logging::errlog("incorrect quantity");
		return -1;
	}
}
