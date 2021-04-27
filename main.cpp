// server가 여러개 띄워져 있어야 하나?
// g_server 초기화 문제 존재.
// server와 client 객체의 역할 분명히 해야 할듯..
// timeout은 어떻게 쓰이는지?

int 	main(int ac, char **av){

	fd_set	readSet;
	fd_set	writeSet;
	fd_set	rSet;
	fd_set  wSet;// 굳이 rSet, wSet 나눠야 하나? 하나로 처리하면 안되나?
	struct timeval		timeout;

	// 매개 변수 확인 후 Conf 파일 파싱, 파싱 중 error 발생 시 예외 처리
	if (ac != 2)
		return (msg_error("Error: Enter config file"));
	try
	{
		// sample code
		// config.parse(av[1], g_servers);
		// config.init(&rSet, &wSet, &readSet, &writeSet, &timeout);
	}
	catch (std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return (1);
	}

	// 샘플 코드
	while (g_state)
	{
		// 초기화
		readSet = rSet;
		writeSet = wSet;
		select(config.getMaxFd(g_servers) + 1, &readSet, &writeSet, NULL, &timeout);
		for (std::vector<Server>::iterator s(g_servers.begin()); s != g_servers.end(); ++s)
		{
			if (FD_ISSET(s->getFd(), &readSet))
			{
				try
				{
					// g_state가 끝나면 바로 종료되도록
					// if (!g_state)
					// 	break ;
					// if (config.getOpenFd(g_servers) > MAX_FD)
					// 	s->refuseConnection();
					// else
					// 	s->acceptConnection();
				}
				catch (std::exception &e)
				{
					std::cerr << "Error: " << e.what() << std::endl;
				}
			}
			if (!s->_tmp_clients.empty())
				if (FD_ISSET(s->_tmp_clients.front(), &writeSet))
					s->send503(s->_tmp_clients.front());// 굳이 send503?

			// 요청이 들어왔을 때, read와 write를 동시에 확인하도록
			for (std::vector<Client*>::iterator c(s->_clients.begin()); c != s->_clients.end(); ++c)
			{
				client = *c;
				if (FD_ISSET(client->fd, &readSet))
					if (!s->readRequest(c))
						break ;
				if (FD_ISSET(client->fd, &writeSet))
					if (!s->writeResponse(c))
						break ;
				if (client->write_fd != -1)
					if (FD_ISSET(client->write_fd, &writeSet))
						client->writeFile();
				if (client->read_fd != -1)
					if (FD_ISSET(client->read_fd, &readSet))
						client->readFile();
			}
		}
	}
	g_servers.clear();
	return(0);

	return 0;
}