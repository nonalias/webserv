#ifndef PARSER_HPP
#define PARSER_HPP

class Parser {
    public:
        Parser();
        ~Parser();
        void		parseRequest(Client &client, std::vector<config> &conf);
        bool		parseHeaders(std::string &buf, Request &req);
        void		parseBody(Client &client);
        void		parseAccept(Client &client, std::multimap<double, std::string> &map, std::string Accept);
        void        parseCGIResult(Client &client);
};

#endif                                                                                  