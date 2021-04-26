#ifndef HANDLER_HPP
#define HANDLER_HPP

#include <vector>
#include <dirent.h>
#include <sys/stat.h>

class Handler
{
	typedef std::map<std::string, std::string> 	elmt;
	typedef std::map<std::string, elmt>			config;

	public:
		Handler();
		~Handler();

	private:
		void			getConf(Client &client, Request &req, std::vector<config> &conf);
};

#endif
