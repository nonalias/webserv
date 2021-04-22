void	Dispatcher::Dispatcher()
{
    // 생성자 관련 코드

//    map["GET"] = &Handler::handleGet;
//    map["HEAD"] = &Handler::handleHead;
//    map["PUT"] = &Handler::handlePut;
//    map["POST"] = &Handler::handlePost;
//    map["CONNECT"] = &Handler::handleConnect;
//    map["TRACE"] = &Handler::handleTrace;
//    map["OPTIONS"] = &Handler::handleOptions;
//    map["DELETE"] = &Handler::handleDelete;
//    map["BAD"] = &Handler::handleBadRequest;

//    map["GET"] = &Dispatcher::GETStatus;
//    map["HEAD"] = &Dispatcher::GETStatus;
//    map["PUT"] = &Dispatcher::PUTStatus;
//    map["POST"] = &Dispatcher::POSTStatus;
//    map["CONNECT"] = &Dispatcher::CONNECTStatus;
//    map["TRACE"] = &Dispatcher::TRACEStatus;
//    map["OPTIONS"] = &Dispatcher::OPTIONSStatus;
//    map["DELETE"] = &Dispatcher::DELETEStatus;

//    MIMETypes[".txt"] = "text/plain";
//    MIMETypes[".bin"] = "application/octet-stream";
//    MIMETypes[".jpeg"] = "image/jpeg";
//    MIMETypes[".jpg"] = "image/jpeg";
//    MIMETypes[".html"] = "text/html";
//    MIMETypes[".htm"] = "text/html";
//    MIMETypes[".png"] = "image/png";
//    MIMETypes[".bmp"] = "image/bmp";
//    MIMETypes[".pdf"] = "application/pdf";
//    MIMETypes[".tar"] = "application/x-tar";
//    MIMETypes[".json"] = "application/json";
//    MIMETypes[".css"] = "text/css";
//    MIMETypes[".js"] = "application/javascript";
//    MIMETypes[".mp3"] = "audio/mpeg";
//    MIMETypes[".avi"] = "video/x-msvideo";
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