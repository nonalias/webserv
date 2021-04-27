#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include "Logger.hpp"

Logger::Logger(bool on, std::string fileName, int param)
: _fileName(fileName), _param(param), _on(false)
{
	if (on)
		_on = true;
}

Logger::~Logger()
{

}

/*!
 * @author	jinbkim
 * @param	text [몇일/몇월/몇년도 몇시:몇분:몇초] [포트번호] listening...
 * @see		Logger::log()
 * @brief	log 정보를 stdout에 출력
 */
void			Logger::logToConsole(const std::string text)
{
	std::cout << text << std::endl;
}

/*!
 * @author	jinbkim
 * @param	text [몇일/몇월/몇년도 몇시:몇분:몇초] [포트번호] listening...
 * @see		Logger::log()
 * @brief	log 정보를 file에 출력
 */
void			Logger::logToFile(const std::string text)
{
	std::ofstream	file;

	file.open(_fileName, std::ofstream::app);
	file << text << std::endl;
	file.close();
}

/*!
 * @author	jinbkim
 * @param	text [포트번호] listening...
 * @return	[몇일/몇월/몇년도 몇시:몇분:몇초] [포트번호] listening...
 * @see		Logger::log()
 * @brief	log 정보를 반환
 */
std::string		Logger::makeLogEntry(const std::string text)
{
	std::string log;

	log += getTimestamp(); // log : [몇일/몇월/몇년도 몇시:몇분:몇초]
	log += " " + text;  // log : [몇일/몇월/몇년도 몇시:몇분:몇초] [포트번호] listening...
	return (log);
}

/*!
 * @author	jinbkim
 * @param	message [포트번호] listening...
 * @param	value ?
 * @see		Client.cpp, Handler.cpp, Server.cpp
 * @brief	log 정보를 stdout or 파일에 출력
 */
void			Logger::log(std::string const &message, int value)
{
	if (_on && value <= _param)  // 무슨 의미? (jinbkim)
	{
		if (_fileName == "console")
			logToConsole(makeLogEntry(message));  // log 정보를 stdout에 출력
		else
			logToFile(makeLogEntry(message));  // log 정보를 파일에 출력
	}
}

/*!
 * @author	jinbkim
 * @return	현재 시간정보 : [몇일/몇월/몇년도 몇시:몇분:몇초]
 * @see		Logger::makeLogEntry()
 * @brief	현재 시간 정보를 string 으로 반환
 */
std::string		Logger::getTimestamp(void)
{
	time_t 		now = 0;
	tm 			*ltm = NULL;
	char		buffer[1024];
	std::string result;
	
	now = time(0);  // now : 1970 1월 1시 0시부터 현재 시간까지 흐른 시간(초)
	if (now)
		ltm = localtime(&now);  // now를 기준으로 tm 구조체 ltm이 초기화됨
	// strftime : 시간을 원하는 형식으로 출력하기위해 사용
	// %d : 몇일
	// %m : 몇월
	// %y : 몇년도
	// %T : 몇시:몇분:몇초
	// buffer : 몇일/몇월/몇년도 몇시:몇분:몇초 가 저장됨
	strftime(buffer, 1024, "%d/%m/%y %T", ltm);
	// reslut : [몇일/몇월/몇년도 몇시:몇분:몇초]
	result = buffer;
	result.insert(result.begin(), '[');
	result.insert(result.end(), ']');
   	return (result);
}