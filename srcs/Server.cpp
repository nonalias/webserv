#include "Server.hpp"
#include "utils.h"

Server::Server() : _fd(-1), _maxFd(-1), _port(-1)
{
	memset(&_info, 0, sizeof(_info));
}

Server::~Server()
{

}

int		Server::getFd() const
{
	return (_fd);
}

int		Server::getMaxFd()
{
	Client	*client;
	for (std::vector<Client*>::iterator it(_clients.begin()); it != _clients.end(); ++it)
	{
		client = *it;
		if (client->read_fd > _maxFd)
			_maxFd = client->read_fd;
		if (client->write_fd > _maxFd)
			_maxFd = client->write_fd;
	}
	return (_maxFd);
}

int		Server::getOpenFd()
{
	int 	nb = 0;
	Client	*client;

	for (std::vector<Client*>::iterator it(_clients.begin()); it != _clients.end(); ++it)
	{
		client = *it;
		nb += 1;
		if (client->read_fd != -1)
			nb += 1;
		if (client->write_fd != -1)
			nb += 1;
	}
	nb += _tmp_clients.size();
	return (nb);
}

void	Server::refuseConnection()
{
	int 				fd = -1;
	struct sockaddr_in	info;
	socklen_t			len;

	errno = 0;
	len = sizeof(struct sockaddr);
	if ((fd = accept(_fd, (struct sockaddr *)&info, &len)) == -1)  // accept 함수호출로 클라이언트의 연결요청을 수락
		throw(ServerException("accept()", std::string(strerror(errno))));
	if (_tmp_clients.size() < 10)
	{
		_tmp_clients.push(fd);
		FD_SET(fd, _wSet);
	}
	else
		close(fd);
}

void	Server::acceptConnection()
{
	int 				fd = -1;
	struct sockaddr_in	info;
	socklen_t			len;
	Client				*newOne = NULL;

	memset(&info, 0, sizeof(struct sockaddr));
	errno = 0;
	len = sizeof(struct sockaddr);
	if ((fd = accept(_fd, (struct sockaddr *)&info, &len)) == -1)  // accept 함수호출로 클라이언트의 연결요청을 수락
		throw(ServerException("accept()", std::string(strerror(errno))));
	if (fd > _maxFd)
		_maxFd = fd;
	newOne = new Client(fd, _rSet, _wSet, info);
	_clients.push_back(newOne);
	g_logger.log("[" + std::to_string(_port) + "] " + "connected clients: " + std::to_string(_clients.size()), LOW); // 연결 요청한 클라이언트 log 정보를 stdout에 출력
}

void	Server::send503(int fd)
{
	Response		response;
	std::string		str;
	int				ret = 0;

	response.version = "HTTP/1.1";
	response.status_code = UNAVAILABLE;
	response.headers["Retry-After"] = RETRY;
	response.headers["Date"] = ft::getDate();
	response.headers["Server"] = "webserv";
	response.body = UNAVAILABLE;
	response.headers["Content-Length"] = std::to_string(response.body.size());
	std::map<std::string, std::string>::const_iterator b = response.headers.begin();
	str = response.version + " " + response.status_code + "\r\n";
	while (b != response.headers.end())
	{
		if (b->second != "")
			str += b->first + ": " + b->second + "\r\n";
		++b;
	}
	str += "\r\n";
	str += response.body;
	ret = write(fd, str.c_str(), str.size());
	if (ret >= -1)
	{
		close(fd);
		FD_CLR(fd, _wSet);
		_tmp_clients.pop();
	}
	g_logger.log("[" + std::to_string(_port) + "] " + "connection refused, sent 503", LOW);  // stdout에 503에러 log 전달
}

int		Server::readRequest(std::vector<Client*>::iterator it)
{
	int 		bytes;
	int			ret;
	Client		*client = NULL;
	std::string	log;

	client = *it;
	bytes = strlen(client->rBuf);
	ret = read(client->fd, client->rBuf + bytes, BUFFER_SIZE - bytes);
	bytes += ret;
	if (ret > 0)
	{
		client->rBuf[bytes] = '\0';
		if (strstr(client->rBuf, "\r\n\r\n") != NULL
			&& client->status != Client::BODYPARSING)
		{
			log = "REQUEST:\n";
			log += client->rBuf;
			g_logger.log(log, HIGH);
			client->last_date = ft::getDate();
			_parser.parseRequest(*client, _conf);
			client->setWriteState(true);
		}
		if (client->status == Client::BODYPARSING)
			_parser.parseBody(*client);
		return (1);
	}
	else
	{
		*it = NULL;
		_clients.erase(it);
		if (client)
			delete client;
		g_logger.log("[" + std::to_string(_port) + "] " + "connected clients: " + std::to_string(_clients.size()), LOW);
		return (0);
	}
}


Server::ServerException::ServerException(void)
{
	this->error = "Undefined Server Exception";
}

Server::ServerException::ServerException(std::string function, std::string error)
{
	this->error = function + ": " + error;
}

Server::ServerException::~ServerException(void) throw() {}

const char			*Server::ServerException::what(void) const throw()
{
	return (this->error.c_str());
}

