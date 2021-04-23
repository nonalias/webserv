#include "utils.h"
#include "Handler.hpp"

Handler::Handler()
{

}

Handler::~Handler()
{

}

void			Handler::getConf(Client &client, Request &req, std::vector<config> &conf)
{
	std::map<std::string, std::string> elmt;
	std::string		tmp;
	std::string		word;
	struct stat		info;
	struct stat		info_tmp;
	config			to_parse;

	if (!req.valid)
	{
		client.conf["error"] = conf[0]["server|"]["error"];
		return ;
	}
	std::vector<config>::iterator it(conf.begin());
	while (it != conf.end())
	{
		if (req.headers["Host"] == (*it)["server|"]["server_name"])
		{
			to_parse = *it;
			break ;
		}
		++it;
	}
	if (it == conf.end())
		to_parse = conf[0];
	tmp = req.uri;
	do
	{
		if (to_parse.find("server|location " + tmp + "|") != to_parse.end())
		{
			elmt = to_parse["server|location " + tmp + "|"];
			break ;
		}
		tmp = tmp.substr(0, tmp.find_last_of('/'));
	} while (tmp != "");
	if (elmt.size() == 0)
		if (to_parse.find("server|location /|") != to_parse.end())
			elmt = to_parse["server|location /|"];
	if (elmt.size() > 0)
	{
		client.conf = elmt;
		client.conf["path"] = req.uri.substr(0, req.uri.find("?"));
		if (elmt.find("root") != elmt.end())
			client.conf["path"].replace(0, tmp.size(), elmt["root"]);
	}
	for (std::map<std::string, std::string>::iterator it(to_parse["server|"].begin()); it != to_parse["server|"].end(); ++it)
	{
		if (client.conf.find(it->first) == client.conf.end())
			client.conf[it->first] = it->second;
	}
	lstat(client.conf["path"].c_str(), &info);
	if (S_ISDIR(info.st_mode))
	{
		if (client.conf["index"][0] && client.conf["listing"] != "on")
		{
			tmp = elmt["index"];
			/* 이부분 수정함 */
			if (client.conf["index"][0] && client.conf["listing"] != "on")
			{
				while (!tmp.empty())
				{
					ft::getline(tmp, word, ' ');
					if (!lstat((client.conf["path"] + "/" + word).c_str(), &info_tmp))
					{
						client.conf["path"] +=  "/" + word;
						break;
					}
				}
			}
		}
	}
	if (req.method == "GET")
		client.conf["savedpath"] = client.conf["path"];
	g_logger.log("path requested from " + client.ip + ":" + std::to_string(client.port) + ": " + client.conf["path"], MED);
}
