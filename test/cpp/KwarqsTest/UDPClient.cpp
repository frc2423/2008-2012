

#include "WPILib.h"

#include "UDPClient.h"



bool KUDPClient::Open(const char * serverName, int port)
{
	if ((sFd = socket (AF_INET, SOCK_DGRAM, 0)) == ERROR) 
    { 
		perror ("socket"); 
        return false; 
    } 
 
    /* explicit bind not required - local port number is dynamic */ 
 
    /* build server socket address */ 
    bzero ((char *) &serverAddr, sizeof (struct sockaddr_in)); 
    serverAddr.sin_len = (u_char) sizeof (struct sockaddr_in); 
    serverAddr.sin_family = AF_INET; 
    serverAddr.sin_port = htons (port); 
 
    if (((serverAddr.sin_addr.s_addr = inet_addr ((char*)serverName)) == ERROR) && 
        ((serverAddr.sin_addr.s_addr = hostGetByName ((char*)serverName)) == ERROR)) 
        { 
    	perror ("unknown server name"); 
        close (sFd); 
        return false; 
    } 
    
    return true;
}

bool KUDPClient::printf(const char * format, ...)
{
	char buf[1024];
	
	va_list ap;
	va_start(ap, format);
	int sz = vsnprintf(buf, sizeof(buf), format, ap);
	
	if (sz > 0)
		return Send(buf, sz);

	return false;
}

bool KUDPClient::Send(const char * buffer, int len)
{
	if (sendto (sFd, (caddr_t) buffer, len, 0, 
			(struct sockaddr *) &serverAddr, sizeof (struct sockaddr_in)) == ERROR)
	{ 
		perror ("sendto"); 
		close (sFd); 
		return false; 
	} 
	
	return true;
}

void KUDPClient::Close()
{
    close (sFd); 	
}


