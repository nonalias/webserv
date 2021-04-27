#ifndef DISPATCHER_HPP
#define DISPATCHER_HPP

#include <sstream>

class Dispatcher
{
    public:
        Dispatcher();
        ~Dispatcher();
        void            execute(Client &client);

    private:
        // Dispatcher
        void	        GETMethod(Client &client);
        void	        HEADMethod(Client &client);
        void	        POSTMethod(Client &client);
        void        	PUTMethod(Client &client);
        void        	CONNECTMethod(Client &client);
        void        	TRACEMethod(Client &client);
        void        	OPTIONSMethod(Client &client);
        void        	DELETEMethod(Client &client);
        void        	handlingBadRequest(Client &client);

        // DispatcherCGI
        char        	**setCGIEnv(Client &client);
        void        	executeCGI(Client &client);
        void          	parseCGIResult(Client &client);

        // DispatcherNegotiate
        void        	negotiate(Client &client);

        // DispatcherStatusCode
        int     		setStatusCode(Client &client);
        int     		GETStatus(Client &client);
        int     		POSTStatus(Client &client);
        int     		PUTStatus(Client &client);
        int     		CONNECTStatus(Client &client);
        int     		TRACEStatus(Client &client);
        int     		OPTIONSStatus(Client &client);
        int	        	DELETEStatus(Client &client);

        // DispatcherUtils
        void			createListing(Client &client);
        void		    createResponse(Client &client);
        std::string		decode64(const char *data);
        std::string		findType(Client &client);
        void			getErrorPage(Client &client);
        std::string		getLastModified(std::string path);
        bool            checkCGI(Client &client);
};
#endif