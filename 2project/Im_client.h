/*
 * im_client
 * CS423 client server applications
 * victoria wu
 *
 * A collection of helper methods, and shared data ie buddy log, messages from
 * other peers.
 */

#ifndef IM_CLIENT_H
#define IM_CLIENT_H

#include <sstream>	//for sending msgs  - change??
#include <string>
#include <map>
#include "MySock.h"
class Im_client	{

	private:

		//MySockets
		MySock peerListener;
		MySock serverListener;

		std::string userName;
		std::map< std::string, std::pair< std::string, std::string> >
			buddyLog;		
	public:
		Im_client();
		~Im_client();

		enum E_MSG_TYPE	{
			LOG_ON = 1,	
			SEND_MSG,
			STATUS_CHK,
			USR_UPDATE,
			FILE_LIST,
			FILE_DL
		};

		//listener fn for threads
		static void listenToServer(void* me);

		void startup(int backlog, std::string serverName, std::string portNum);
		void logOn(std::string name);
		void shutdown();

};

#endif
