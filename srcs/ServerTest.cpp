#include "Server.hpp"

extern	std::vector<Server> g_servers;

void	Server::print_conf(void)
{
	for (std::vector<Server>::iterator it(g_servers.begin()); it != g_servers.end(); ++it)
	{
		for(std::vector<config>::iterator conf_iter=it->_conf.begin(); conf_iter!=it->_conf.end(); conf_iter++)
		{
			for (config::iterator key_iter=conf_iter->begin(); key_iter!=conf_iter->end(); key_iter++)
			{
				std::cout << key_iter->first << std::endl;
				for(elmt::iterator elmt_iter=key_iter->second.begin(); elmt_iter!=key_iter->second.end(); elmt_iter++)
				{
					std::cout << "\t\t\t" << elmt_iter->first << "\t" << elmt_iter->second << std::endl;
				}
			}
			std::cout << "\n";
		}
		std::cout << "\n===========================================================\n";
	}
}