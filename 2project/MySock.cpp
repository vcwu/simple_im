/*
 * MySock
 * CS423 - client server applications
 * spring 2013
 * victoria wu
 */

#include <iostream>	//cerr, cout
#include <string>
#include "MySock.h"
/*
 * Creates a socket of specified transport type.
 * modified from Comer's Internetworking with TCP/IP Volume III, section 7.7
 */
MySock::MySock(std::string transport)	{

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
