#include "utils.h"
#include "Client.hpp"
# include <sys/stat.h>
# include <vector>


Client::Client(int filed, fd_set *r, fd_set *w, struct sockaddr_in info)
: fd(filed), read_fd(-1), write_fd(-1), status(STANDBY), cgi_pid(-1), tmp_fd(-1), rSet(r), wSet(w)
{
	ip = inet_ntoa(info.sin_addr);  // inet_ntoa : 네트워크 바이트 순서의 32비트 형태인 IP주소를 문자열로 변환
	port = htons(info.sin_port);  // htons : short형 호스트 바이트 순서를 네트워크 바이트 순서로 변환
	rBuf = (char *)malloc(sizeof(char) * (BUFFER_SIZE + 1));
	memset((void *)rBuf, 0, BUFFER_SIZE + 1);
	fcntl(fd, F_SETFL, O_NONBLOCK); // fd를 non-blocking 모드로 바꿈
	FD_SET(fd, rSet);  // 클라이언트와 연결된 서버의 rSet에 클라이언트 fd 추가
	FD_SET(fd, wSet);  // 클라이언트와 연결된 서버의 wSet에 클라이언트 fd 추가
	chunk.len = 0;
	chunk.done = false;
	chunk.found = false;
	last_date = ft::getDate();  // 몇요일/몇일/몇월(영어)/몇년도 몇시:몇분:몇초 timezone
	g_logger.log("new connection from " + ip + ":" + std::to_string(port), LOW);  // 연결 요청한 클라이언트 log 정보를 stdout에 출력
}

Client::~Client()
{
	free(rBuf);
	rBuf = NULL;
	if (fd != -1)
	{
		close(fd);
		FD_CLR(fd, rSet);
		FD_CLR(fd, wSet);
	}
	if (read_fd != -1)
	{
		close(read_fd);
		FD_CLR(read_fd, rSet);
	}
	if (write_fd != -1)
	{
		close(write_fd);
		FD_CLR(write_fd, wSet);	
	}
	if (tmp_fd != -1)
	{
		close(tmp_fd);
		unlink(TMP_PATH);
	}
	g_logger.log("connection closed from " + ip + ":" + std::to_string(port), LOW);
}

void	Client::setReadState(bool state)
{
	if (state)
		FD_SET(fd, rSet);
	else
		FD_CLR(fd, rSet);
}

void	Client::setWriteState(bool state)
{
	if (state)
		FD_SET(fd, wSet);
	else
		FD_CLR(fd, wSet);
}

void	Client::setFileToRead(bool state)
{
	if (read_fd != -1)
	{
		if (state)
			FD_SET(read_fd, rSet);
		else
			FD_CLR(read_fd, rSet);
	}
}

void	Client::setFileToWrite(bool state)
{
	if (write_fd != -1)
	{
		if (state)
			FD_SET(write_fd, wSet);
		else
			FD_CLR(write_fd, wSet);
	}
}

void	Client::readFile()
{
	char			buffer[BUFFER_SIZE + 1];
	int				ret = 0;
	int				status = 0;

	if (cgi_pid != -1)
	{
		if (waitpid((pid_t)cgi_pid, (int *)&status, (int)WNOHANG) == 0) // 종료되었는지 체크. 안 되었으면 반환값은 0
			return ;
		else
		{
			if (WEXITSTATUS(status) == 1) // 비정상종료. 프로그램이 정상적으로 끝난 반환값은 기본적으로 0이다.
			{
				close(tmp_fd);
				tmp_fd = -1;
				cgi_pid = -1;
				close(read_fd);
				unlink(TMP_PATH);
				setFileToRead(false);
				read_fd = -1;
				res.body = "Error with cgi.\n";
				return ;
			}
		}
	}
	ret = read(read_fd, buffer, BUFFER_SIZE); // getErrorPage로 가져온 read_fd를 읽는다.

	if (ret >= 0)
		buffer[ret] = '\0';
	std::string	tmp(buffer, ret);
	res.body += tmp;
	if (ret == 0)
	{
		close(read_fd);
		unlink(TMP_PATH);
		setFileToRead(false);
		read_fd = -1;
	}
}

void	Client::writeFile()
{
	int ret = 0;

	ret = write(write_fd, req.body.c_str(), req.body.size());
	if (cgi_pid != -1)
		g_logger.log("sent " + std::to_string(ret) + " bytes to CGI stdin", MED);
	else
		g_logger.log("write " + std::to_string(ret) + " bytes in file", MED);
	if ((unsigned long)ret < req.body.size())
		req.body = req.body.substr(ret);
	else
	{
		req.body.clear();
		close(write_fd);
		setFileToWrite(false);
		write_fd = -1;
	}
}

void	Client::setToStandBy()
{
	g_logger.log(req.method + " from " + ip + ":" + std::to_string(port) + " answered", MED);
	status = STANDBY;
	setReadState(true);
	if (read_fd != -1)
		close(read_fd);
	read_fd = -1;
	if (write_fd != -1)
		close(write_fd);
	write_fd = -1;
	memset((void *)rBuf, (int)0, (size_t)(BUFFER_SIZE + 1));
	conf.clear();
	req.clear();
	res.clear();
}

void			Client::getConf(Client &client, Request &req, std::vector<config> &conf)
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
