/*
 * MySock
 * CS423 - client server applications
 * spring 2013
 * victoria wu
 */

#define DEBUG

#include <iostream>	//cerr, cout
#include <string>
#include "MySock.h"
/*
 * Creates a socket of specified transport type.
 * modified from Comer's Internetworking with TCP/IP Volume III, section 7.7
 */
MySock::MySock(std::string transport)	{

	std::cout << "CEREATING SOCKET " << std::endl;
	struct protoent *ppe;	// pointer to protocol information entr
	int type;	//type of service
	//map protocol name to protocol number, define type
	ppe = getprotobyname(transport.c_str());
	if(strcmp(transport.c_str(), "udp") == 0 )	
		type = SOCK_DGRAM;
	else
		type = SOCK_STREAM;

	//Finally, allocate a socket.
	s = socket(PF_INET, type, ppe->p_proto);
	if(s == INVALID_SOCKET)	{
		std::cerr << "can't create socket";
		WSACleanup();
		exit(1);
	}
	#ifdef DEBUG
	std::cout << "Socket created successfully" << std::endl;

	#endif
}

/*
 * ~MySock
 */

MySock::~MySock()	{
	closesocket(s);
}

/* connectToHost
 * modified from Comer's Internetworking with TCP/IP Volume III, section 7.7
 * @param serverName server IP address in dotted-decimal format
 * @param portNum 
 */
void MySock::connectToHost(std::string serverName, std::string portNum)	{
	
	struct sockaddr_in sin;	// an internet endpoint address

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons((u_short)atoi(portNum.c_str())); 
	sin.sin_addr.s_addr = (inet_addr(serverName.c_str()));

	//Connect socket.
	if(connect(s, (struct sockaddr*) &sin, sizeof(sin))==SOCKET_ERROR) 	
	{
		std::cerr << "can't connect to " << serverName << " " << portNum;
		WSACleanup();
		exit(1);
	}

	std::cout << "Socket Creation and connection successful" <<std::endl;


}

/*
 * startListening
 * Binds this socket to a randomly assigned port, then starts listening.
 * Code from Beej's guide to Network Programming
 *
 * @param backlogSize 
 */
void MySock::startListening(int backlogSize)	{
	struct sockaddr_in my_addr;
	
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = 0;			//choose an unused port 
	my_addr.sin_addr.s_addr = INADDR_ANY;	
	memset(& (my_addr.sin_zero), '\0', 8);	//zero the rest of struct

	//Binding socket
	if( bind(s, (struct sockaddr*) &my_addr, sizeof(struct sockaddr)) == SOCKET_ERROR)	{
		std::cerr << "Cannot bind socket " << s << "to port" << std::endl;
	}
	
	//Getting port number
	int size_my_addr = sizeof(struct sockaddr);
	if(getsockname(s, (struct sockaddr*) &my_addr, &size_my_addr)
			== SOCKET_ERROR)	{
		std::cerr << "Could not retrieve port num for socket " << s <<
			std::endl;
	}
	
	port = my_addr.sin_port;
#ifdef DEBUG
	std::cout << "Bound socket " << s << " to port " << port << std::endl;
#endif

	//Start listening
	if( listen(s, backlogSize) == SOCKET_ERROR)	{
		std::cerr << "Cannot listen on socket " << s<< std::endl;
	}
#ifdef DEBUG
	std::cout << "Socket " << s << " is listening on port " << port << std::endl;
#endif
}







