#ifndef UTILS_H
# define UTILS_H

# include <string>
# include <sys/time.h>
#include <string>
#include <iostream>
#include "Config.hpp"
# include "Server.hpp"

namespace ft
{
	std::string &trim(std::string &str);
	bool		isspace(int c);
	void		getline(std::string &buffer, std::string &line);
	void		getline(std::string &buffer, std::string &line, char delim);
	int			getpower(int nb, int power);
	std::string	getDate();
	void		freeAll(char **args, char **env);
	int			getMaxFd(std::vector<Server> &servers);
	int			getOpenFd(std::vector<Server> &servers);
}

#endif
