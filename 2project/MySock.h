/*
 * MySock 
 * spring 2013
 * victoria wu
 * cs 423 - client server applications
 *
 * Class that wraps basic functionality for setting up, reading and writing to socket.
 *
 */


#ifndef MY_SOCK_H
#define MY_SOCK_H

#include <winsock.h>
#include <string>
//#pragma comment(lib, "wsock32.lib")	//link winsock lib

class MySock {
	
	private:
		SOCKET s;
		unsigned short port;	//for easier access
	public:
		MySock(std::string transport = "tcp");	//defaults to TCP
		~MySock();

		unsigned short getLocalPort();
		SOCKET getSocket()	{return s;}
		void startListening(int backlogSize, u_short port);
		void connectToHost(std::string serverName, std::string portNum);

		
		bool sendMsg(std::string message);
//		recv(std::string buffer);
};

#endif /*MY_SOCK*/
