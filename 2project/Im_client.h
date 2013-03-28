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
#include "MySock.h"
class Im_client	{

	private:

		//MySockets
		MySock peerListener;
		MySock serverListener;

		std::string userName;
	public:
		Im_client();
		~Im_client();

		void startup(int backlog, std::string serverName, std::string portNum);
		void logOn(std::string name);
};

#endif
