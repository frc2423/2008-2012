
#include <stdio.h>

#include "DataServer.h"

#include <windows.h>

int main()
{

	IntProxy i1 = DataServer::CreateIntProxy("group", "i1", 
					IntProxyFlags().default_value(5).minval(0).maxval(10).step(1) );
	IntProxy i2 = DataServer::CreateIntProxy("group", "i2",
					IntProxyFlags().default_value(6).minval(0).maxval(10).step(1));

	FloatProxy f1 = DataServer::CreateFloatProxy("group", "f1", 
					FloatProxyFlags().default_value(7).minval(0).maxval(10).step(1) );
	FloatProxy f2 = DataServer::CreateFloatProxy("group", "f2",
					FloatProxyFlags().default_value(.3F).minval(0).maxval(1).step(.01F));
					
	BoolProxy stayOn = DataServer::CreateBoolProxy("Controls", "Turn server off", true);
	
	DataServer::Enable();
	
	while (stayOn)
	{
		printf("%5d %5d %5.1f %5.1f\r", (int)i1, (int)i2, (float)f1, (float)f2);
	
		Sleep(100);
	}

	return true;
	
}