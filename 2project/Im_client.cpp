#define DEBUG

#include "Im_client.h"
#include "process.h"	//multi threading
#include <iostream>
Im_client::Im_client() : userName(""), peerListener("tcp"),
	serverListener("tcp")	{}

Im_client::~Im_client()	{

}

void Im_client::startup(int backlog, std::string serverName, std::string portNum){
	//Start listening for peer requests.
	#ifdef DEBUG
	std::cout << "Starting up peer listener on socket: " <<
		peerListener.getSocket() <<std::endl;;
	#endif
	peerListener.startListening(backlog);
	//START UP THEREADL:FJKDL:SKFJD

	#ifdef DEBUG
	std::cout << "Starting up connection to server on socket: " <<
		serverListener.getSocket() << std::endl;
	#endif
//Connect to server and start listening.
	serverListener.connectToHost(serverName, portNum);
	_beginthread(listenToServer, 0, (void *) this);	
}


/*
 * Sends a log on message to server. 
 */

void Im_client::logOn(std::string name)	{
	#ifdef DEBUG
	std::cout << "Signing in as " << name << std::endl;
	#endif

	
	std::stringstream ss;
	ss << "1;" << name << ";" << peerListener.getLocalPort() << "#";
	std::string message = ss.str();
	
	serverListener.sendMsg(message);

}

/*
 * Thread fn to listen for replies from server.
 */
void Im_client::listenToServer(void * me)	{
	Im_client* box = (Im_client*)me;
	SOCKET serverSock = box->serverListener.getSocket();

	#ifdef DEBUG
	std::cout << "FROM THREAD 1: Listening for server messages on socket" << serverSock << std::endl;
	#endif


	const int bufferLength = 550;
	char recvbuf[bufferLength];

	while(true)	{
		memset(recvbuf, '\0', bufferLength);
		if(recv(serverSock, recvbuf, bufferLength, 0) == SOCKET_ERROR)
		{
			std::cerr << "ERROR in receiving message from server" << std::endl;	
		}
		else	{
			std::string msg(recvbuf);
			std::cout << "GOT: " << msg << std::endl;
		}
	}
}
