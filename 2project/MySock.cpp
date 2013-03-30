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
 * Return local port that socket is bound to.
 */
unsigned short MySock::getLocalPort()	{
	if(port == 0)	{	
		//Getting port number
		struct sockaddr_in my_addr;
		int size_my_addr = sizeof(struct sockaddr);
		if(getsockname(s, (struct sockaddr*) &my_addr, &size_my_addr)
				== SOCKET_ERROR)	{
			std::cerr << "Could not retrieve port num for socket " << s <<
				std::endl;
		}
		
		port = my_addr.sin_port;
	}
	return port;
}

/*
 * Creates a socket of specified transport type.
 * Does not bind to a port.
 * modified from Comer's Internetworking with TCP/IP Volume III, section 7.7
 */
MySock::MySock(std::string transport = "tcp")	{

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
		std::cerr << "can't allocate socket";
		WSACleanup();
		exit(1);
	}
	
	//Clear out port field.
	port = 0;

	#ifdef DEBUG
	std::string s_type;
	(type == SOCK_DGRAM) ? s_type = "udp" : s_type = "tcp";
	std::cout << "Socket type: " << s_type << " num: " << s << " allocated successfully" <<
		std::endl;
	#endif
}

/*
 * ~MySock
 */

MySock::~MySock()	{
	#ifdef DEBUG
	std::cout << "Closing socket " << s << std::endl;
	#endif
	closesocket(s);
}

/* connectToHost
 * modified from Comer's Internetworking with TCP/IP Volume III, section 7.7
 * @param serverName server IP address in dotted-decimal format
 * @param portNum 
 */
void MySock::connectToHost(std::string serverName, std::string portNum)	{
	
	struct sockaddr_in sin;	// an internet endpoint address
	struct hostent *phe;	//pointer to host info entry
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons((u_short)atoi(portNum.c_str())); 

	if( phe = gethostbyname(serverName.c_str()))
		memcpy(&sin.sin_addr, phe->h_addr, phe->h_length);
	else if( sin.sin_addr.s_addr = htons(inet_addr(serverName.c_str())) ==
		INADDR_NONE)
		std::cerr << "can't get host entry for " << serverName;

	//Connect socket.
	if(connect(s, (struct sockaddr*) &sin, sizeof(sin))==SOCKET_ERROR) 	
	{
		std::cerr << "can't connect to " << serverName << " " << portNum;
		std::cout << "errno: " << WSAGetLastError();
		WSACleanup();
		exit(1);
	}

	#ifdef DEBUG
	std::cout << "Socket " << s << " connected successfullly to " <<
		serverName << " port: " << portNum <<std::endl;
	#endif

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

	//Find out port number
	unsigned short port = getLocalPort();	
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


/*
 * send
 * Send given message to connected host. Should not be called on 
 * listening sockets.
 */
void MySock::sendMsg(std::string message)	{
	
	#ifdef DEBUG
	std::cout << "Preparing to send on socket " << s << std::endl;
	std::cout << "message: " << message << std::endl;
	#endif

	int bytes_sent = send(s, message.c_str(), strlen(message.c_str() ), 0);
	if(bytes_sent == SOCKET_ERROR)	{
		std::cerr << "Error in sending message\n" << message <<
			std::endl;
	}
	
	#ifdef DEBUG
	std::cout << "Successfully sent msg" <<  message;
	#endif
}




