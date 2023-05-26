#ifndef __CONNECTION_IF_HPP__
#define __CONNECTION_IF_HPP__

#include "status.hpp"

class connection_if
{
public:
	virtual status read(std::string &message) = 0;
	virtual status write(std::string_view message) = 0;
};

#endif // !__CONNECTION_IF_HPP__
