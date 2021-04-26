int			Dispatcher::setStatusCode(Client &client)
{
    std::string                 credential;
    int							ret;

    if (client.req.method != "CONNECT"
        && client.req.method != "TRACE"
        && client.req.method != "OPTIONS")
    {
        client.res.version = "HTTP/1.1";
        client.res.status_code = OK;
        if (client.conf["methods"].find(client.req.method) == std::string::npos)
            client.res.status_code = NOTALLOWED;
        else if (client.conf.find("auth") != client.conf.end())
        {
            client.res.status_code = UNAUTHORIZED;
            if (client.req.headers.find("Authorization") != client.req.headers.end())
            {
                credential = decode64(client.req.headers["Authorization"].c_str());
                if (credential == client.conf["auth"])
                    client.res.status_code = OK;
            }
        }
    }
    ret = (this->*status[client.req.method])(client);
    if (ret == 0)
        getErrorPage(client);
    return (ret);
}

int			Dispatcher::GETStatus(Client &client)
{
    struct stat		info;

    if (client.res.status_code == OK)
    {
        errno = 0;
        client.read_fd = open(client.conf["path"].c_str(), O_RDONLY);
        if (client.read_fd == -1 && errno == ENOENT)
            client.res.status_code = NOTFOUND;
        else if (client.read_fd == -1)
            client.res.status_code = INTERNALERROR;
        else
        {
            fstat(client.read_fd, &info);
            if (!S_ISDIR(info.st_mode)
                || (S_ISDIR(info.st_mode) && client.conf["listing"] == "on"))
                return (1);
            else
                client.res.status_code = NOTFOUND;
        }
    }
    return (0);
}

int			Dispatcher::POSTStatus(Client &client)
{
    int				fd;
    struct stat		info;

    if (client.res.status_code == OK && client.conf.find("max_body") != client.conf.end()
        && client.req.body.size() > (unsigned long)atoi(client.conf["max_body"].c_str()))
        client.res.status_code = REQTOOLARGE;
    if (client.res.status_code == OK)
    {
        if (client.conf.find("CGI") != client.conf.end()
            && client.req.uri.find(client.conf["CGI"]) != std::string::npos)
        {
            if (client.conf["exec"][0])
                client.read_fd = open(client.conf["exec"].c_str(), O_RDONLY);
            else
                client.read_fd = open(client.conf["path"].c_str(), O_RDONLY);
            fstat(client.read_fd, &info);
            if (client.read_fd == -1 || S_ISDIR(info.st_mode))
                client.res.status_code = INTERNALERROR;
            else
                return (1);
        }
        else
        {
            errno = 0;
            fd = open(client.conf["path"].c_str(), O_RDONLY);
            if (fd == -1 && errno == ENOENT)
                client.res.status_code = CREATED; // 생성되지 않았을텐데? CREATED를 넣네? 왜냐면 있든 없든 이따 create할건데 지금 체크해야하는건 새로 만들어지는건지 아님 기존거에 append되는건지이기 때문
            else if (fd != -1)
                client.res.status_code = OK; // 오픈 성공해서 fd값 얻어왔으면 OK. 즉 이미 존재함
            close(fd); // 방금 연 파일 닫아주고
            client.write_fd = open(client.conf["path"].c_str(), O_WRONLY | O_APPEND | O_CREAT, 0666); // 이번엔 O_APPEND| O_CREAT플래그를 넣어서 이미 있으면 있는대로 연장해서 내용 써지고 없으면 만들어줄수있도록 open함
            if (client.write_fd == -1)
                client.res.status_code = INTERNALERROR; // 위 open이 실패하는건 예상치못한 내부적인 오류임. INTERNALERROR넣어줌
            else
                return (1);
        }
    }
    return (0); // status_code가 NOTALLOWED, UNAUTHORIZED, REQTOOLARGE인경우 0 리턴
}

int			Dispatcher::PUTStatus(Client &client)
{

}

int			Dispatcher::CONNECTStatus(Client &client)
{

}

int			Dispatcher::TRACEStatus(Client &client)
{

}

int			Dispatcher::OPTIONSStatus(Client &client)
{

}

int			Dispatcher::DELETEStatus(Client &client)
{

}
