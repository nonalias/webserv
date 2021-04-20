#include "Config.hpp"

std::vector<Server>		g_servers;
bool					g_state = true;

int		ret_error(std::string error)
{
	std::cerr << error << std::endl;
	return (1);
}

int 	main(int ac, char **av)
{
	Config					config;
	Server					tmpServer;

	if (ac != 2)
		return (ret_error("Usage: ./webserv config-file"));
	try
	{
		config.parse(av[1], g_servers);
		// config.init(&rSet, &wSet, &readSet, &writeSet, &timeout);
		tmpServer.print_conf();
	}
	catch (std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return (1);
	}

	g_servers.clear();
	return(0);
}
