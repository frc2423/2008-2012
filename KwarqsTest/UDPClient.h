

#ifndef KWARQS_UDPCLIENT_H
#define KWARQS_UDPCLIENT_H

#include "sockLib.h"

#include "hostLib.h"
#include "inetLib.h"

class KUDPClient {
public:
	
	KUDPClient() { sFd = 0; }
	
	bool Open(const char * serverName, int port);
	
	bool printf(const char *fmt, ...);
	bool Send(const char *, int len);
	
	void Close();
	
private:
	
	int sFd;
	struct sockaddr_in  serverAddr; 
	
};

#endif
