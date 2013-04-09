/*
 * cs 423 client server applications
 * victoria wu
 * IM_Client implementation
 */

//#define DEBUG

#include "Im_client.h"
#include "process.h"	//multi threading
#include <utility>	//make pair
#include <map>		//buddy log
#include <iostream>
#include <iomanip>	//setw, setfill
#include <sstream>
Im_client::Im_client() : userName(""), peerListener("tcp"),
	serverListener("tcp")	{
		closingSocketTime = false;
		InitializeCriticalSection(&critSec);
		InitializeConditionVariable(&threadsDown);
	}

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

	closingSocketTime = true;
	//may need to wait a bit?

	EnterCriticalSection( & critSec);
	while(	!readyToShutdown)	{
		SleepConditionVariableCS(&threadsDown, &critSec, INFINITE);
	}
	LeaveCriticalSection(& critSec);

	std::cout << "Threads all ok to shutdown! " << std::endl;

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

	while(!box->closingSocketTime)	{
		memset(recvbuf, '\0', bufferLength);
		if(recv(serverSock, recvbuf, bufferLength, 0) == SOCKET_ERROR)
		{
			if(box->closingSocketTime)	{
				break;
			}
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
	EnterCriticalSection(&(box->critSec));	
	box->readyToShutdown = true;
	LeaveCriticalSection(&(box->critSec));
	WakeConditionVariable( &(box->threadsDown));
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


//***************************************
//USER INTERACTION
//***************************************

/*
 * displayMenu
 */
void Im_client::displayMenu()	{
	std::cout << "What would you like to do?" << std::endl;
//	std::cout << "\tCheck for messages (c)" << std::endl;
	std::cout << "\tSend msg to buddy (s)" << std::endl;
	std::cout << "\tGet file list from buddy (f)" << std::endl;
	std::cout << "\tDownload file from buddy (d)" << std::endl;
	std::cout << "\tQuit (q)" << std::endl;
}

/*
 * sendMessage()
 * Asks user for message and recipient, sends.
 */
void Im_client::sendMessage()	{
	std::string buffer;
	std::string message, buddy;
	std::cout << "Recipient: ";
	std::cin >> buddy;
	std::cout << "Message: ";
	std::cin >> message;

	std::stringstream ss;
	ss << SEND_MSG << ";" << userName << "/n"
		<< buddy << "/n" << message << "#"; 
	
	
	if(!sendToBuddy(buddy, ss.str()))	{
		std::cout << " Unable to send to " << buddy << std::endl;
	}
	else	{
		std::cout << "Sent msg successfully to " << buddy << std::endl;
	}

}

/*
 * getFileNames
 */
void Im_client::getFileNames()	{
	std::string buddy;
	std::cout << "Who would you like to get files from? ";
	std::cin >> buddy;
	std::stringstream ss;
	ss << FILE_LIST << ";fileList/n";

	if(!sendToBuddy(buddy, ss.str()))	{
		std::cout << " Unable to send to " << buddy << std::endl;
	}
	else	{
		std::cout << "File list requested from " << buddy << std::endl;
	}

}

/*
 * downloadFile
 */
void Im_client::downloadFile()	{
	std::cout << "let's download a file - yohoo! " << std::endl;
}

/**
 * sendToBuddy
 */
bool Im_client::sendToBuddy(std::string buddy, std::string msg)	{
	BuddyLog::iterator it;
	it = log.find(buddy);
	if(it == log.end())	{
		std::cout << buddy << " is not logged on. " << std::endl;
		return false;
	}
	else	{
		MySock buddy;
		std::string ip = it->second.first;
		std::string port = it->second.second;
		buddy.connectToHost(ip, port);
		buddy.sendMsg(msg);
	}
	return true;	
}
