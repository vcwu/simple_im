#define DEBUG

#include "Im_client.h"
#include "process.h"	//multi threading
#include <utility>	//make pair
#include <map>		//buddy log
#include <iostream>
#include <sstream>
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
 * Shut down - Shuts down sockets.
 * Should probably kill threads too.
 */
void Im_client::shutdown()	{
	peerListener.~MySock();
	serverListener.~MySock();
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
			#ifdef DEBUG
			std::string msg(recvbuf);
			std::cout << "GOT: " << msg << std::endl;
			#endif

			//Only check for #4 messages, update buddy log.
			std::stringstream ss(msg);
			std::string code, count;
			std::getline(ss, code,';');
			
			std::string userName, ip, port;
			if(atoi(code.c_str()) == USR_UPDATE)	{ 
				std::getline(ss, count, '\n');
				int userCount = atoi(count.c_str());

				if(userCount == -1)	{
					getline(ss, userName, ';');
					getline(ss, ip, ';');
					getline(ss, port, '#');

					//Make sure this user is in our
					//buddylog.
					if(box->log.find(userName) !=
							box->log.end())
						box->log.erase (box->log.find(userName));
				}
				
				for(int userNum = 1; userNum <= userCount; userNum++)		{
					getline(ss, userName, ';');
					getline(ss, ip, ';');
					if(userNum == userCount)
						getline(ss, port, '#');	//END  of message
						
					else	{
						getline(ss, port, '\n');
					}

					box->log[userName] = std::make_pair(ip, port);
				}
			}
			
			#ifdef DEBUG
			std::cout << "Parsing: count = " << count << std::endl;
			std::cout << "userName = " << userName << std::endl;
			std::cout << "port = " << port << std::endl;
			std::cout << "ip = " <<ip << std::endl;
			#endif

		}
	}
}
