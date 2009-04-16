
#include <stdio.h>

#include "src/WebInterface.h"

#ifdef _MSC_VER
	#include <windows.h>
#else
	#include <unistd.h>
#endif

int main()
{

	IntProxy i1 = WebInterface::CreateIntProxy("group", "i1", 
					IntProxyFlags().default_value(5).minval(0).maxval(10).step(1) );
	IntProxy i2 = WebInterface::CreateIntProxy("group", "i2",
					IntProxyFlags().default_value(6).minval(0).maxval(10).step(1));

	FloatProxy f1 = WebInterface::CreateFloatProxy("group", "f1", 
					FloatProxyFlags().default_value(7).minval(0).maxval(10).step(1) );
	FloatProxy f2 = WebInterface::CreateFloatProxy("group", "f2",
					FloatProxyFlags().default_value(.3F).minval(0).maxval(1).step(.01F));

	DoubleProxy d1 = WebInterface::CreateDoubleProxy("group", "d1",
					DoubleProxyFlags().default_value(10).minval(-180).maxval(180).step(1));
					
	BoolProxy stayOn = WebInterface::CreateBoolProxy("Controls", "Turn server off", true);
	
	WebInterface::Enable();
	
	while (stayOn)
	{
		printf("%5d %5d %5.1f %5.1f %5.1f\r", (int)i1, (int)i2, (float)f1, (float)f2, (double)d1);
	
		
#ifdef _MSC_VER
		Sleep(100);
#else
		usleep(10000);
#endif
	}

	return true;
	
}