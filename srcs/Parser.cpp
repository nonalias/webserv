#include "Parser.hpp"
# include "utils.h"


Parser::Parser()
{

}

Parser::~Parser()
{

}

bool			Parser::checkSyntax(const Request &req)
{
	if (req.method.size() == 0 || req.uri.size() == 0
		|| req.version.size() == 0)
		return (false);
	if (req.method != "GET" && req.method != "POST"
		&& req.method != "HEAD" && req.method != "PUT"
		&& req.method != "CONNECT" && req.method != "TRACE"
		&& req.method != "OPTIONS" && req.method != "DELETE")
		return (false);
	if (req.method != "OPTIONS" && req.uri[0] != '/') //OPTIONS can have * as uri option OPTIONS 만 / 가 uri로 안 와도 된다. 나머지는 모두 /부터 시작해야한다.
		return (false);
	if (req.version != "HTTP/1.1\r" && req.version != "HTTP/1.1") // version 규약
		return (false);
	if (req.headers.find("Host") == req.headers.end()) // headers에 Host가 없으면
		return (false);
	return (true);
}

void			Parser::getClientConf(Client &client, Request &req, std::vector<config> &conf)
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
        getClientConf(client, request, conf);
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

void			Parser::getBody(Client &client)
{
	unsigned int	bytes;

	if (client.chunk.len == 0) // 길이가 0이면
		client.chunk.len = atoi(client.req.headers["Content-Length"].c_str()); // client.req.headers[Content-Length]의 value를 숫자로 치환.
	if (client.chunk.len < 0) // 0보다 작으면 BAD
	{
		client.req.method = "BAD";
		client.status = Client::CODE;
		return ;
	}
	bytes = strlen(client.rBuf); // rBuf 길이
	if (bytes >= client.chunk.len) // rBuf길이가 Content-Length보다 길면 읽을 만큼만 client.req.body에 받아옴
	{
		memset(client.rBuf + client.chunk.len, 0, BUFFER_SIZE - client.chunk.len); // client.chunk.len 뒤의 rBuf를 다 초기화
		client.req.body += client.rBuf; // rBuf를 넣어줌
		client.chunk.len = 0; // 초기화
		client.status = Client::CODE;
	}
	else // rBuf길이보다 읽어야 할 길이가 길면
	{
		client.chunk.len -= bytes; // client.chunk.len에서 rBuf의 길이만큼 뺌 = 못 읽는 문자열 길이 -> 이거 초기화를 왜 안 시킬까? 다음 getBody할 때 이 길이만큼 받아올 수 있도록 하기 위해.
		client.req.body += client.rBuf; // rBuf 전체를 client.req.body에 넣어줌
		memset(client.rBuf, 0, BUFFER_SIZE + 1); // 메모리 초기화
	}
}

int				Parser::findLen(Client &client)
{
	std::string		to_convert;
	int				len;
	std::string		tmp;

	to_convert = client.rBuf;
	to_convert = to_convert.substr(0, to_convert.find("\r\n"));	//첫 번째 문장 파싱
	while (to_convert[0] == '\n')
		to_convert.erase(to_convert.begin());	//공백 라인 제거
	if (to_convert.size() == 0)
		len = 0;
	else
		len = fromHexa(to_convert.c_str());		//16진수 문자열 -> 10진수화
	len = fromHexa(to_convert.c_str());			//???
	tmp = client.rBuf;							//다음 문장으로 이동
	tmp = tmp.substr(tmp.find("\r\n") + 2);
	strcpy(client.rBuf, tmp.c_str());
	return (len);
}

int				Parser::fromHexa(const char *nb)
{
	char	base[17] = "0123456789abcdef";
	char	base2[17] = "0123456789ABCDEF";
	int		result = 0;
	int		i;
	int		index;

	i = 0;
	while (nb[i])
	{
		int j = 0;
		while (base[j])
		{
			if (nb[i] == base[j]) // "0123456789abcdef" 중 일치하는 문자가 있을 경우
			{
				index = j;
				break ;
			}
			j++;
		}
		if (j == 16) // "0123456789abcdef" 중 일치하는 문자가 없었을 경우, 즉 대문자일 경우
		{
			j = 0;
			while (base2[j])
			{
				if (nb[i] == base2[j]) // "0123456789ABCDEF" 중 일치하는 문자가 있을 경우
				{
					index = j;
					break ;
				}
				j++;
			}
		}
		result += index * ft::getpower(16, (strlen(nb) - 1) - i);
		i++;
	}
	return (result);
}

void			Parser::fillBody(Client &client)
{
	std::string		tmp;

	tmp = client.rBuf; // chunk_size\r\n(client.chunk.len) 다음 rBuf
	if (tmp.size() > client.chunk.len) // read 버퍼의 크기가 chunk 길이보다 길 때
	{
		client.req.body += tmp.substr(0, client.chunk.len); // read 버퍼의 내용을 chunk 길이만큼 request body에 붙여줌
		tmp = tmp.substr(client.chunk.len + 1);
		memset(client.rBuf, 0, BUFFER_SIZE + 1);
		strcpy(client.rBuf, tmp.c_str()); // request body에 붙인 이후 내용을 read 버퍼에 복사함
		client.chunk.len = 0;
		client.chunk.found = false;
	}
	else
	{
		client.req.body += tmp; // read 버퍼의 내용을 request body에 붙여줌
		client.chunk.len -= tmp.size(); // chunk 길이를 read 버퍼의 사이즈만큼 줄여줌
		memset(client.rBuf, 0, BUFFER_SIZE + 1);
	}
}

void			Parser::dechunkBody(Client &client)
{
	if (strstr(client.rBuf, "\r\n") && client.chunk.found == false) // rBuf에 \r\n이 있고 client.chunk.found가 false면
	{
		client.chunk.len = findLen(client); // chunk_size 문자열로 되어있는 것을 실제 숫자로 변환. client의 rBuf는 \r\n 다음으로 이동.
		if (client.chunk.len == 0) // 길이가 0이면 끝.
			client.chunk.done = true;
		else // 길이가 있으면 찾음을 알림
			client.chunk.found = true;
	}
	else if (client.chunk.found == true) // /r/n을 찾으면
		fillBody(client); // client.req.body에 내용을 채움 client.rBuf는 읽은 만큼 넘어감.
	if (client.chunk.done) // chunk 완료. client.chunk.len = 0
	{
		memset(client.rBuf, 0, BUFFER_SIZE + 1); // 초기화
		client.status = Client::CODE;
		client.chunk.found = false; // chunk가 끝났으므로 초기화
		client.chunk.done = false;
		return ;
	}
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
        if (to_parse[i])
            ++i;
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
