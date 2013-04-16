
#define WSVERS MAKEWORD(2,0)
#define DEBUG

//#include "MySock.h"
#include <sstream>
#include <cstdlib>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <winsock.h>

#pragma comment(lib, "wsock32.lib")
#pragma comment(lib, "libcmt.lib")


using namespace std;

SOCKET passiveSock(string service, string transport, int qlen)	{
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
	return s;
}

int main()	{
	
	WSADATA wsadata;
	if(WSAStartup(WSVERS, &wsadata) != 0)	{
		std::cerr << "WSA Startup failed " << std::endl;
		exit(1);
	}
	
//	MySock yo;
//	yo.startListening(10, 34567);


/* STUFF DIRECTLY FROM COMER
*/


	SOCKET yo = passiveSock("34567", "tcp", 10);

	cout << "passive sock call success" << endl;
	struct sockaddr their_addr;
	int addr_len = sizeof(their_addr);
	while(true)	{
		SOCKET fd;
		fd = accept(yo, &their_addr, &addr_len);
//		MySock slave(fd);
//		slave.sendMsg("Howdy there!");		
	}
	WSACleanup();
}
