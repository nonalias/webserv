#include "Parser.hpp"

void			Parser::parseRequest(Client &client, std::vector<config> &conf)
{
    Request				request;
    std::string			tmp;
    std::string			buffer;

    buffer = std::string(client.rBuf);
    if (buffer[0] == '\r')
        buffer.erase(buffer.begin());
    if (buffer[0] == '\n')
        buffer.erase(buffer.begin());
    ft::getline(buffer, request.method, ' ');
    ft::getline(buffer, request.uri, ' ');
    ft::getline(buffer, request.version);
    if (parseHeaders(buffer, request))
        request.valid = checkSyntax(request);
    if (request.uri != "*" || request.method != "OPTIONS")
        getConf(client, request, conf);
    if (request.valid)
    {
        if (client.conf["root"][0] != '\0')
            chdir(client.conf["root"].c_str());
        if (request.method == "POST" || request.method == "PUT")
            client.status = Client::BODYPARSING;
        else
            client.status = Client::CODE;
    }
    else
    {
        request.method = "BAD";
        client.status = Client::CODE;
    }
    client.req = request;
    tmp = client.rBuf;
    tmp = tmp.substr(tmp.find("\r\n\r\n") + 4);
    strcpy(client.rBuf, tmp.c_str());
}

bool			Parser::parseHeaders(std::string &buf, Request &req)
{
    size_t		pos;
    std::string	line;
    std::string	key;
    std::string	value;

    while (!buf.empty())
    {
        ft::getline(buf, line);
        if (line.size() < 1 || line[0] == '\n' || line[0] == '\r')
            break ;
        if (line.find(':') != std::string::npos)
        {
            pos = line.find(':');
            key = line.substr(0, pos);
            if (line[pos + 1] == ' ')
                value = line.substr(pos + 2);
            else
                value = line.substr(pos + 1);
            if (ft::isspace(value[0]) || ft::isspace(key[0]) || value.empty() || key.empty())
            {
                req.valid = false;
                return (false);
            }
            req.headers[key] = value;
            req.headers[key].pop_back(); //remove '\r'
        }
        else
        {
            req.valid = false;
            return (false);
        }
    }
    return (true);
}

void			Parser::parseBody(Client &client)
{
    if (client.req.headers.find("Content-Length") != client.req.headers.end())
        getBody(client);
    else if (client.req.headers["Transfer-Encoding"] == "chunked")
        dechunkBody(client);
    else
    {
        client.req.method = "BAD";
        client.status = Client::CODE;
    }
    if (client.status == Client::CODE)
        g_logger.log("body size parsed from " + client.ip + ":" + std::to_string(client.port) + ": " + std::to_string(client.req.body.size()), MED);
}

void			Parser::parseAccept(Client &client, std::multimap<double, std::string> &map, std::string Accept)
{
    std::string							language;
    std::string							to_parse;
    std::string					tmp;
    std::stringstream           tmp2;
    double                              q;

    to_parse = client.req.headers[Accept];
    int i = 0;
    while (to_parse[i] != '\0')
    {
        language.clear();
        tmp2.clear();
        tmp.clear();
        q = 0;
        while (to_parse[i] && to_parse[i] != ',' && to_parse[i] != ';')
            language += to_parse[i++];
        if (to_parse[i] == ',' || to_parse[i] == '\0')
            tmp = "1";
        else if (to_parse[i] == ';')
        {
            i += 3;
            while (to_parse[i] && to_parse[i] != ',')
                tmp += to_parse[i++];
        }
        tmp2 << tmp;
        tmp2 >> q;
        while (to_parse[i] == ',' || to_parse[i] == ' ')
            i++;
        std::pair<double, std::string>	pair(q, language);
        map.insert(pair);
    }
}

void		Parser::parseCGIResult(Client &client)
{
    size_t			pos;
    std::string		headers;
    std::string		key;
    std::string		value;

    if (client.res.body.find("\r\n\r\n") == std::string::npos)
        return ;
    headers = client.res.body.substr(0, client.res.body.find("\r\n\r\n") + 1);
    pos = headers.find("Status");
    if (pos != std::string::npos)
    {
        client.res.status_code.clear();
        pos += 8;
        while (headers[pos] != '\r')
        {
            client.res.status_code += headers[pos];
            pos++;
        }
    }
    pos = 0;
    while (headers[pos])
    {
        while (headers[pos] && headers[pos] != ':')
        {
            key += headers[pos];
            ++pos;
        }
        ++pos;
        while (headers[pos] && headers[pos] != '\r')
        {
            value += headers[pos];
            ++pos;
        }
        client.res.headers[key] = value;
        key.clear();
        value.clear();
        if (headers[pos] == '\r' || headers[pos] == '\n')
            pos++;
    }
    pos = client.res.body.find("\r\n\r\n") + 4;
    client.res.body = client.res.body.substr(pos);
    client.res.headers["Content-Length"] = std::to_string(client.res.body.size());
}
