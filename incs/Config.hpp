#ifndef CONFIG_HPP
# define CONFIG_HPP

#include "package.hpp"

// # include <fcntl.h>
// # include <unistd.h>
// # include <stdlib.h>
// # include <string>
// # include <map>
// # include <vector>
// # include <iostream>
// # include <dirent.h>
// # include "Server.hpp"
// # include "Client.hpp"

// # include <sys/stat.h>
#define MAX_FD 256 - 20

class Config
{
    typedef std::map<std::string, std::string> 	elmt;
    typedef std::map<std::string, elmt>			config;

    public:
        Config(Dispatcher &_dispatcher);
        ~Config();

        static void	exit(int sig);
        void		parse(char *file, std::vector<Server> &servers);
        void		init(fd_set *rSet, fd_set *wSet, fd_set *readSet, fd_set *writeSet, struct timeval *timeout);

    class InvalidConfigFileException: public std::exception
    {
        private:
            size_t						line;
            std::string					error;

            InvalidConfigFileException(void);

        public:
            InvalidConfigFileException(size_t d);
            virtual ~InvalidConfigFileException(void) throw();
            virtual const char			*what(void) const throw();
    };

    private:
        std::string	readFile(char *file);
        void		getContent(std::string &buffer, std::string &context, std::string prec, size_t &nb_line, config &config);
        Dispatcher  &_dispatcher;
};

#endif
