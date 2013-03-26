/*
 * MySock 
 * spring 2013
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
	public:
		MySock(std::string transport);
		~MySock();
	
		void connectToHost(std::string serverName, std::string portNum);
	


//		send(std::string message);
//		recv(std::string buffer);
};

#endif /*MY_SOCK*/
