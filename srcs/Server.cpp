#include "utils.h"
#include "Server.hpp"

Server::Server() : _fd(-1), _maxFd(-1), _port(-1)
{
	memset(&_info, 0, sizeof(_info));
}

Server::~Server()
{

}

Server::ServerException::ServerException(void)
{
	this->error = "Undefined Server Exception";	
}

Server::ServerException::ServerException(std::string function, std::string error)
{
	this->error = function + ": " + error;
}

Server::ServerException::~ServerException(void) throw() {}

const char			*Server::ServerException::what(void) const throw()
{
	return (this->error.c_str());
}
