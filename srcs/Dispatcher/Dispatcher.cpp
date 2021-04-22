Dispatcher::Dispatcher()
{
    typedef void	(Dispatcher::*ptr)(Client &client);

    std::map<std::string, ptr> method;
    std::map<std::string, ptr> status;
    std::map<std::string, std::string> MIMETypes;

    method["GET"] = &Dispatcher::GETMethod;
    method["HEAD"] = &Dispatcher::HEADMethod;
    method["PUT"] = &Dispatcher::PUTMethod;
    method["POST"] = &Dispatcher::POSTMethod;
    method["CONNECT"] = &Dispatcher::CONNECTMethod;
    method["TRACE"] = &Dispatcher::TRACEMethod;
    method["OPTIONS"] = &Dispatcher::OPTIONSMethod;
    method["DELETE"] = &Dispatcher::DELETEMethod;
    method["BAD"] = &Dispatcher::handlingBadRequest;

    status["GET"] = &Dispatcher::GETStatus;
    status["HEAD"] = &Dispatcher::GETStatus;
    status["PUT"] = &Dispatcher::PUTStatus;
    status["POST"] = &Dispatcher::POSTStatus;
    status["CONNECT"] = &Dispatcher::CONNECTStatus;
    status["TRACE"] = &Dispatcher::TRACEStatus;
    status["OPTIONS"] = &Dispatcher::OPTIONSStatus;
    status["DELETE"] = &Dispatcher::DELETEStatus;

    MIMETypes[".txt"] = "text/plain";
    MIMETypes[".bin"] = "application/octet-stream";
    MIMETypes[".jpeg"] = "image/jpeg";
    MIMETypes[".jpg"] = "image/jpeg";
    MIMETypes[".html"] = "text/html";
    MIMETypes[".htm"] = "text/html";
    MIMETypes[".png"] = "image/png";
    MIMETypes[".bmp"] = "image/bmp";
    MIMETypes[".pdf"] = "application/pdf";
    MIMETypes[".tar"] = "application/x-tar";
    MIMETypes[".json"] = "application/json";
    MIMETypes[".css"] = "text/css";
    MIMETypes[".js"] = "application/javascript";
    MIMETypes[".mp3"] = "audio/mpeg";
    MIMETypes[".avi"] = "video/x-msvideo";
}

void    Dispatcher::execute(Client &client)
{
    (this->*map[client.req.method])(client);
}

void	Dispatcher::GETMethod(Client &client)
{

}

void	Dispatcher::HEADMethod(Client &client)
{

}

void	Dispatcher::POSTMethod(Client &client)
{

}

void	Dispatcher::PUTMethod(Client &client)
{

}

void	Dispatcher::CONNECTMethod(Client &client)
{

}

void	Dispatcher::TRACEMethod(Client &client)
{

}

void	Dispatcher::OPTIONSMethod(Client &client)
{

}

void	Dispatcher::DELETEMethod(Client &client)
{

}

void	Dispatcher::handlingBadRequest(Client &client)
{

}