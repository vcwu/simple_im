/*
 * cs 423 client server applications
 * victoria wu
 * IM_Client implementation
 */

#define DEBUG

#include "Im_client.h"
#include "process.h"	//multi threading
#include <utility>	//make pair
#include <map>		//buddy log
#include <iostream>
#include <iomanip>	//setw, setfill
#include <sstream>
Im_client::Im_client() 	{	
		userName = "";
		closingSocketTime = false;
		InitializeCriticalSection(&critSec);
		InitializeConditionVariable(&threadsDown);
	}

Im_client::~Im_client()	{

}


SOCKET Im_client::passiveSock(std::string service, std::string transport, int qlen)	{
	struct servent	*pse;	/* pointer to service information entry	*/
	struct protoent *ppe;	/* pointer to protocol information entry*/
	struct sockaddr_in sin;	/* an Internet endpoint address		*/
	int	s, type;	/* socket descriptor and socket type	*/

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;

    /* Map service name to port number */
	if ( pse = getservbyname(service.c_str(), transport.c_str()) )
		sin.sin_port = htons(ntohs((u_short)pse->s_port));
	else if ( (sin.sin_port = htons((u_short)atoi(service.c_str()))) == 0 )
		printf("can't get \"%s\" service entry\n", service);

    /* Map protocol name to protocol number */
	if ( (ppe = getprotobyname(transport.c_str())) == 0)
		printf("can't get \"%s\" protocol entry\n", transport);

    /* Use protocol to choose a socket type */
	if (strcmp(transport.c_str(), "udp") == 0)
		type = SOCK_DGRAM;
	else
		type = SOCK_STREAM;

    /* Allocate a socket */
	s = socket(PF_INET, type, ppe->p_proto);
	if (s < 0)
		printf("can't create socket: %s\n", strerror(errno));

    /* Bind the socket */
	if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
		printf("can't bind to %s port: %s\n", service,
			strerror(errno));
	if (type == SOCK_STREAM && listen(s, qlen) < 0)
		printf("can't listen on %s port: %s\n", service,
			strerror(errno));
	#ifdef DEBUG
	std::cout << "Successfully created passive socket sock: " <<s << " port: " << service << std::endl;
	#endif
	return s;
}



void Im_client::startup(int backlog, std::string serverName, std::string
		portNum, std::string port){
	//Start listening for peer requests.
//	peerListener.startListening(backlog, port);
	peerListener.setSocket(passiveSock(port, "tcp", backlog));
//	_beginthread(listenToPeers, 0, (void *) this);	

	#ifdef DEBUG
	std::cout << "Started up peer listener on socket: " <<
		peerListener.getSocket() <<std::endl;;
	#endif

	//Connect to server and start listening.
	if(!serverListener.connectToHost("tcp", serverName, portNum))	{
		std::cerr << "Unable to connect to Server. Goodbye." << std::endl;
		WSACleanup();
		exit(1);
	}
	_beginthread(listenToServer, 0, (void *) this);	

	#ifdef DEBUG
	std::cout << "Starting up connection to server on socket: " <<
		serverListener.getSocket() << std::endl;
	#endif

}

/*
 * Shut down - Shuts down sockets.
 * Should probably kill threads too.
 */
void Im_client::shutdown()	{

	#ifdef DEBUG
	std::cout << "Trying to Shutdown... " << std::endl;
	#endif
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
				#ifdef DEBUG
				std::cout << "From THREAD 1: Time for Shutdown!! " << std::endl; 
				#endif
				break;
			}
			std::cerr << "ERROR in receiving message from server" << std::endl;	
			std::cerr << "Error# " << WSAGetLastError() <<
				std::endl;
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
				//sometimes foundIndex gets to be waaaaay out. ??
				substrLen = foundIndex - beginIndex + 1;
				littleMsg = msg.substr(beginIndex, substrLen);
				box->parseServerMsg(littleMsg);
				beginIndex = foundIndex + 1;
				#ifdef DEBUG
				std::cout << "foundIndex: " << foundIndex; 
				std::cout << " substrLen: " << substrLen;
				std::cout << " MSG:: " << littleMsg; 
				#endif
			} while(beginIndex < msg.size() - 1 && !(foundIndex>550));
		}
	}
	#ifdef DEBUG
	std::cout << "Setting readyToShutdown flag..." << std::endl;
	#endif 
	EnterCriticalSection(&(box->critSec));	
	box->readyToShutdown = true;
	LeaveCriticalSection(&(box->critSec));
	WakeConditionVariable( &(box->threadsDown));
	#ifdef DEBUG
	std::cout << "THREAD 1 exiting...." << std::endl;
	#endif 

}

/*
 * Thread fn to listen for replies from other users.
 * Reference: Beej's guide To Networking
 */
void Im_client::listenToPeers(void * me)	{
	Im_client* box = (Im_client*)me;
	SOCKET peerListen = box->peerListener.getSocket();

	#ifdef DEBUG
	std::cout << "FROM THREAD 2: Listening for peer messages on socket"
		<< peerListen << std::endl;
	#endif


	const int bufferLength = 550;
	char recvbuf[bufferLength];

	struct sockaddr their_addr;
	int addr_len = sizeof( their_addr);
	while(!box->closingSocketTime)	{
		memset(recvbuf, '\0', bufferLength);
		SOCKET new_fd = accept(peerListen, &their_addr, &addr_len);
		if(new_fd == SOCKET_ERROR)
		{
			if(box->closingSocketTime)	{
				#ifdef DEBUG
				std::cout << "From THREAD 2: Time for Shutdown!! " << std::endl; 
				#endif
				break;
			}
			std::cerr << "ERROR in accepting connection from peer" << std::endl;	
			std::cerr<< "errno: " << WSAGetLastError()<< std::endl;
		}
		else	{
			if(recv(peerListen, recvbuf, bufferLength, 0) == SOCKET_ERROR)	{
				std::cerr << "ERROR in recv msg from peer" <<
					std::endl;
			}
			else	{
				std::string msg(recvbuf);
				#ifdef DEBUG
				std::cout << "GOT: " << msg << std::endl <<std::endl;
				#endif

				/*
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
				} while(beginIndex < msg.size() - 1);
				*/
			}
		}
	}
	#ifdef DEBUG
	std::cout << "Setting readyToShutdown flag..." << std::endl;
	#endif 

	//TODO TODO TODO
	//Make this a semaphore. can't just use one bool for two threads
	/*
	EnterCriticalSection(&(box->critSec));	
	box->readyToShutdown = true;
	LeaveCriticalSection(&(box->critSec));
	WakeConditionVariable( &(box->threadsDown));
	*/
	#ifdef DEBUG
	std::cout << "THREAD 2 exiting...." << std::endl;
	#endif 

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
	//Thread time! Yeah!
	//
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
		if(buddy.connectToHost("tcp", ip, port))	{
			buddy.sendMsg(msg);
		}
		else	{
			return false;
		}
		//send msg returns false...??
		//need better error handling
	}
	return true;	
}
