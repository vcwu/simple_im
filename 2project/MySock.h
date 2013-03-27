/*
 * MySock 
 * spring 2013
 * victoria wu
 * cs 423 - client server applications
 *
 * Class that wraps basic functionality for setting up, reading and writing to socket.
 *
 */


#ifndef MY_SOCK
#define MY_SOCK

#include <winsock.h>
#include <string>
//#pragma comment(lib, "wsock32.lib")	//link winsock lib

class MySock {
	
	private:
		SOCKET s;
		unsigned short port;	//for easier access
	public:
		MySock(std::string transport);
		~MySock();

		unsigned short getLocalPort();

		void startListening(int backlogSize);
		void connectToHost(std::string serverName, std::string portNum);
	
		void sendMsg(std::string message);
//		recv(std::string buffer);
};

#endif /*MY_SOCK*/
