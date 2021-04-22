#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <fcntl.h>
# include <unistd.h>
# include <stdlib.h> 
# include <string>
# include <map>
# include <vector>
# include <iostream>
# include "Server.hpp"

# define MAX_FD 256 - 20 // fd에서 20을 뺀 이유?

class Config {
    typedef std::map<std::string, std::string>  elmt;
    typedef std::map<std::string, elmt>         config;
public:
    Config();
    virtual ~Config();

    static void exit(int sig);

    void        parse(char *file, std::vector<Server> &servers);
    void        init(fd_set *rSet, fd_set *wSet, fd_set *readSet, fd_set writeSet, timeval *timeout);
    int         getMaxFd(std::vector<Server> &servers);
    int         getOpenFd(std::vector<Server> &servers);

private:
    std::string readFile(char *file);
    void        getContent(std::string &buffer, std::string &content, std::string prec, size_t &nb_line, config &config);
};


# endif;