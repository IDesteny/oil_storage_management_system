#include <string>
#include "client.hpp"

int main(int argc, char **argv)
{
	client(std::stoull(argv[1])).run();
}
