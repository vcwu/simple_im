#include "Im_client.h"

Im_client::Im_client() : userName(""), peerListener("tcp"),
	serverListener("tcp")	{}

Im_client::~Im_client()	{

}

void Im_client::startup(int backlog, std::string serverName, std::string portNum){
	//Start listening for peer requests.
	peerListener.startListening(backlog);
	//START UP THEREADL:FJKDL:SKFJD

	//Connect to server and start listening.
	serverListener.connectToHost(serverName, portNum);
	//start UP THERADL:FJDSKL:FJSDL:FKJL:SDKFJKL:sd
}


/*
 * Sends a log on message to server. 
 */

void Im_client::logOn(std::string name)	{
	#ifdef DEBUG
	printf("Signing in as %s", name);
	#endif
	
	std::stringstream ss;
	ss << "1" << name << peerListener.getLocalPort() << "#";
	std::string message = ss.str();
	
	serverListener.sendMsg(message);

}

