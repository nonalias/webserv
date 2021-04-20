#include <string>
#include <iostream>
#include "Config.hpp"
#include "Server.hpp"

namespace ft
{
	std::string &trim(std::string &str)
	{
		if (!str.empty())
		{
			while (isspace(str[0]))
				str.erase(str.begin());
			while (isspace(str[str.size() - 1]))
				str.erase(str.end() - 1);
		}
		return (str);
	}

	bool	isspace(int c)
	{
		if (c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f' ||
c == ' ')
			return (true);
		return (false);
	}

	void	getline(std::string &buffer, std::string &line)
	{
		size_t					pos;

		pos = buffer.find("\n");
		if (pos != std::string::npos)
		{
			line = std::string (buffer, 0, pos++);
			buffer = buffer.substr(pos);
		}
		else
		{
			if (buffer[buffer.size() - 1] == '\n')
				buffer = buffer.substr(buffer.size());
			else
			{
				line = buffer;
				buffer = buffer.substr(buffer.size());
			}
		}
	}

	void	getline(std::string &buffer, std::string &line, char delim)
	{
		size_t					pos;

		pos = buffer.find(delim);
		if (pos != std::string::npos)
		{
			line = std::string (buffer, 0, pos++);
			buffer = buffer.substr(pos);
		}
		else
		{
			if (buffer[buffer.size() - 1] == delim)
				buffer = buffer.substr(buffer.size());
			else
			{
				line = buffer;
				buffer = buffer.substr(buffer.size());
			}
		}
	}

	void	freeAll(char **args, char **env)
	{
		free(args[0]);
		free(args[1]);
		free(args);
		int i = 0;
		while (env[i])
		{
			free(env[i]);
			++i;
		}
		free(env);
	}
}
