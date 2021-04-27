#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <sys/wait.h>
#include <string.h>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>
#include <arpa/inet.h>
//#include <string>		iostream에 내장되어있기때문에 없어도 괜찮을듯
#include <map>

#include "messages.h"  // request, response구조체 가져옴.
#include "Logger.hpp"
#include "Parser.hpp"

#define BUFFER_SIZE 32768 // 왜 32768,,?
#define TMP_PATH "/tmp/cgi.tmp" // cgi.tmp파일로 테스트용인가? 깃헙파일에는 없음

extern Logger g_logger; // 로그찍어주는거? 두고


class Client
{
	friend class Server;
	// friend class Handler;
	// friend class Helper;
	// Server, Handler, Helper 클래스에게 private정보 접근권한 열어줌
	friend class Parser;

	typedef std::map<std::string, std::string> t_conf; // 문자열:문자열 맵 t_conf
    typedef std::map<std::string, std::string> 	elmt;
    typedef std::map<std::string, elmt>	config;

	struct t_chunk // 길이, done, found여부 담는 구조체
	{
		unsigned int len;
		bool done;
		bool found;
	};

public:
	enum status // 0 ~ 9까지 상태 저장
	{
		PARSING,
		BODYPARSING,
		CODE,
		HEADERS,
		CGI,
		BODY,
		RESPONSE,
		STANDBY, // 클라이언트의 초기 상태
		DONE
	};
	int fd;
	int read_fd; // 클라이언트 객체의 읽기전용 파일디스크립터
	int write_fd; // 클라이언트 객체의 쓰기전용 파일디스크립터

	void readFile(); // cgi_pid를 기다리는데 WNOHANG옵션으로 기다림 (종료되지 않아서 즉시 종료상태를 회수 할 수 없는 상황에서 호출자는 차단되지 않고 반환값 0을 받는다..?)
	// read_fd의 내용을 buffer에 담고 그 내용은 결국 res.body에 적힘
	void writeFile(); // req.body 문자열을 write_fd에 씀

private:
	int port; // htons(info.sin_port);
	int status; // 자식의 종료값. WEXITSTATUS쓸때 값 받아주는 변수. 1이면 cgi에러
	int cgi_pid;
	int tmp_fd;
	char *rBuf;
	fd_set *rSet;
	fd_set *wSet;
	Request req;
	Response res;
	std::string ip; // 클라이언트 객체 생성시 inet_ntoa(info.sin_addr) 내용 받음
	std::string last_date; // 클라이언트 객체 생성시 ft::getDate() 내용 받음
	std::string response; //
	t_conf conf; // string:string map
	t_chunk chunk; // int len, bool done, bool found 구조체

public:
	Client(int filed, fd_set *r, fd_set *w, struct sockaddr_in info); //
	~Client();

	void setReadState(bool state); // state에 따라 rSet을 FD_SET, FD_CLR
	void setWriteState(bool state); // state 에 따라 wSet을 FD_SET, FD_CLR
	void setFileToRead(bool state); // read_fd -1 아닌지 확인, state에 따라 FD_SET, FD_CLR
	void setFileToWrite(bool state); // write_fd -1 아닌지 확인, state에 따라 FD_SET, FD_CLR
	void setToStandBy(); // status 를 standby로, setReadState, rBuf 0으로 memset. rbuf는 일단 client.cpp에선 쓰이는곳 없음.

	char *getRBuf(void);
};

#endif
