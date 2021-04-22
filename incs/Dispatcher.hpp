#ifndef DISPATCHER_HPP
#define DISPATCHER_HPP

class Dispatcher
{
    public:
        void	        Dispatcher::GETMethod(Client &client);
        void	        Dispatcher::HEADMethod(Client &client);
        void	        Dispatcher::POSTMethod(Client &client);
        void        	Dispatcher::PUTMethod(Client &client);
        void        	Dispatcher::CONNECTMethod(Client &client);
        void        	Dispatcher::TRACEMethod(Client &client);
        void        	Dispatcher::OPTIONSMethod(Client &client);
        void        	Dispatcher::DELETEMethod(Client &client);
        void        	Dispatcher::handlingBadRequest(Client &client);

    private:
        // DispatcherCGI
        char        	**Dispatcher::setEnv(Client &client);
        void        	Dispatcher::execCGI(Client &client);
        void          	Dispatcher::parseCGIResult(Client &client);

        // DispatcherNegotiate
        void        	Dispatcher::negotiate(Client &client);
        void         	Dispatcher::parseAcceptLanguage(Client &client, std::multimap<std::string, std::string> &map);
        void        	Dispatcher::parseAcceptCharset(Client &client, std::multimap<std::string, std::string> &map);

        // DispatcherStatusCode
        int     		Dispatcher::setStatusCode(Client &client);
        int     		Dispatcher::GETStatus(Client &client);
        int     		Dispatcher::POSTStatus(Client &client);
        int     		Dispatcher::PUTStatus(Client &client);
        int     		Dispatcher::CONNECTStatus(Client &client);
        int     		Dispatcher::TRACEStatus(Client &client);
        int     		Dispatcher::OPTIONSStatus(Client &client);
        int	        	Dispatcher::DELETEStatus(Client &client);

        // DispatcherUtils
        void			Dispatcher::createListing(Client &client);
        void		    Dispatcher::createResponse(Client &client);
        std::string		Dispatcher::decode64(const char *data);
        std::string		Dispatcher::findType(Client &client);
        void			Dispatcher::getErrorPage(Client &client);
        std::string		Dispatcher::getLastModified(std::string path);

};
#endif