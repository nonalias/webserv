#ifndef Server_HPP
#define Server_HPP

#include <queue>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include "Client.hpp"
#include "statusCode.h"
#include "Parser.hpp"

#define TIMEOUT 10
#define RETRY	"25"
#define _503_CLIENTS_SIZE 10

class Server
{
    friend class Config;
    friend class Dispatcher;
    typedef std::map<std::string, std::string> 	elmt;
    typedef std::map<std::string, elmt>			config;

    private:
        int						_fd;
        int						_maxFd;
        int						_port;
        struct sockaddr_in		_info;
        fd_set					*_readSet;
        fd_set					*_writeSet;
        fd_set					*_rSet;
        fd_set					*_wSet;
        std::vector<config>		_conf;
        Parser                  _parser;

    public:
        Server();
        ~Server();

		std::vector<Client*>	_clients;
		std::queue<int>			_503_clients;

        // test method
        void print_conf(void);

		int		getFd() const;
        int     getMaxFd();
        int     getOpenFd();
        void    refuseConnection();
        void    acceptConnection();
        void    send503(int fd);
        int     readRequest(std::vector<Client*>::iterator it);
        void	init(fd_set *readSet, fd_set *writeSet, fd_set *rSet, fd_set *wSet);

    class		ServerException: public std::exception
    {
        private:
            std::string function;
            std::string error;

        public:
            ServerException(void);
            ServerException(std::string function, std::string error);
            virtual	~ServerException(void) throw();
            virtual const char		*what(void) const throw();
    };

private:
    int		getTimeDiff(std::string start);
};

#endif
