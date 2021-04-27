#include "Config.hpp"
#include "Logger.hpp"
#include "utils.h"

std::vector<Server>		g_servers;
bool					g_state = true;
Logger					g_logger(1, "console", LOW);

int		ret_error(std::string error)
{
	std::cerr << error << std::endl;
	return (1);
}

int 	main(int ac, char **av)
{
	Config					config;
	Server					tmpServer;
	fd_set					readSet;
	fd_set					writeSet;
	fd_set					rSet;
	fd_set					wSet;
	struct timeval			timeout;
	Client					*client;


	if (ac != 2)
		return (ret_error("Usage: ./webserv config-file"));
	try
	{
		config.parse(av[1], g_servers);
		// config.init(&rSet, &wSet, &readSet, &writeSet, &timeout);
		tmpServer.print_conf();
	}
	catch (std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return (1);
	}
	while (g_state)
	{
		readSet = rSet;
		writeSet = wSet;

		select(ft::getMaxFd(g_servers) + 1, &readSet, &writeSet, NULL, &timeout);
		// select 호출 이후 : readSet 또는 writeSet에서 변화가 있는 fd만 set이 되있음

		for (std::vector<Server>::iterator s(g_servers.begin()); s != g_servers.end(); ++s)
		{
			if (FD_ISSET(s->getFd(), &readSet))  // 해당 서버의 서버 소켓이 데이터를 읽었다면
			{
				try
				{
					if (!g_state)
						break ;
					if (ft::getOpenFd(g_servers) > MAX_FD)  // 모든 서버에 열려있는 fd 수가 MAX_FD 보다 크면
						s->refuseConnection();  // 클라이언트의 연결 요청을 대기하거나 거부
					else
						s->acceptConnection();  // 클라이언트의 연결 요청을 수락
				}
				catch (std::exception &e)
				{
					std::cerr << "Error: " << e.what() << std::endl;
				}
			}
			if (!s->_tmp_clients.empty())
			{
				if (FD_ISSET(s->_tmp_clients.front(), &writeSet))  // 필요없는 조건문 인거 같아서 나중에 주석처리 예정
					s->send503(s->_tmp_clients.front());  // 접속 대기중인 클라이언트에 503 에러 출력
			}

			for (std::vector<Client*>::iterator c(s->_clients.begin()); c != s->_clients.end(); ++c)
			{
				client = *c;  // 해당 서버의 하나의 클라이언트
				if (FD_ISSET(client->fd, &readSet))  // 클라이언트가 서버로 데이터를 전달할 때
					if (!s->readRequest(c))  // 클라이언트가 보낸 request 메시지를 읽고 파싱. 다읽었다면 0을 반환하여 반복문 탈출.
						break ;
				// if (FD_ISSET(client->fd, &writeSet))
				// 	if (!s->writeResponse(c))
				// 		break ;
				// if (client->write_fd != -1)
				// 	if (FD_ISSET(client->write_fd, &writeSet))
				// 		client->writeFile();
				// if (client->read_fd != -1)
				// 	if (FD_ISSET(client->read_fd, &readSet))
				// 		client->readFile(); // 에러메세지도 들어옴 getErrorPage()
			}
		}
	}
	g_servers.clear();
	return(0);
}
