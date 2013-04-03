//#define DEBUG

#include "Im_client.h"
#include "process.h"	//multi threading
#include <utility>	//make pair
#include <map>		//buddy log
#include <iostream>
#include <iomanip>	//setw, setfill
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
	std::cout << "Signing in as " << name << std::endl;

	std::stringstream ss;
	ss << "1;" << name << ";" << std::setw(5) << std::setfill('0') << peerListener.getLocalPort() << "#";
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
			#ifdef DEBUG
			std::cout << "GOT: " << msg << std::endl <<std::endl;
			#endif
	
			//Delimit messages by #. && wait for entire message??
			int beginIndex = 0;
			unsigned foundIndex = 0; 
			int substrLen;
			std::string littleMsg;
			do{
				foundIndex = msg.find('#', beginIndex);
				substrLen = foundIndex - beginIndex + 1;
				littleMsg = msg.substr(beginIndex, substrLen);
				box->parseServerMsg(littleMsg);
				beginIndex = foundIndex + 1;
				#ifdef DEBUG
				std::cout << "foundIndex: " << foundIndex; 
				std::cout << " substrLen: " << substrLen;
				std::cout << " MSG:: " << littleMsg; 
				#endif
			} while(beginIndex < msg.size());
		}
	}
}



/*
 * Given a message, if it is a type 4, will parse active user list and update
 * buddy.
 * Otherwise, ignores the message.
 * log.
 */
void Im_client::parseServerMsg(std::string msg)	{
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
			if(log.find(userName) !=
					log.end())
				log.erase (log.find(userName));
			std::cout << userName << "just logged off." << std::endl;
		}
		
		for(int userNum = 1; userNum <= userCount; userNum++)		{
			getline(ss, userName, ';');
			getline(ss, ip, ';');
			if(userNum == userCount)
				getline(ss, port, '#');	//END  of message
				
			else	{
				getline(ss, port, '\n');
			}

			std::pair<BuddyLog::iterator, bool> pairIt;
			UsrInfo info = std::make_pair(ip, port);
			pairIt = log.insert(std::make_pair(userName, info));
			
			//If user already exists in buddylog,
			//update their info
			if(!pairIt.second)	{
				log[userName] = info;	
			}
			std::cout << userName << " just logged on." << std::endl;
			std::cout << "Buddies: \n";
			BuddyLog::iterator it;
			for(it = log.begin(); it!= log.end(); ++it)	{
				std::cout << it->first << " => " << 
					it->second.first << " " <<
					it->second.second << std::endl;
			}
			std::cout << std::endl;
		}
			

	}
}
