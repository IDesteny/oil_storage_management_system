#include "client.hpp"
#include "logging.hpp"

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		logging::errlog("it is necessary to specify the id of the tank to connect to the arguments");
		return -1;
	}
	
	try
	{
		switch (auto result = client(std::stoull(argv[1])).run())
		{
			case status::failed_initialization:
			{
				logging::errlog("error initializing new session");
				return -1;
			}
			
			case status::failed_accepted:
			{
				logging::errlog("error connecting to the server");
				return -1;
			}
			
			case status::write_error:
			{
				logging::errlog("error writing message to server");
				return -1;
			}
			
			case status::read_error:
			{
				logging::errlog("error reading message from server");
				return -1;
			}
			
			case status::disconnect:
			{
				logging::inflog("ending a session");
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
		logging::errlog("invalid ID");
		return -1;
	}
}
