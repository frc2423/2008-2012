#include <WPILib.h>

float d_err(float number)
{
	if(number < 0.0 && number > -0.000001) return -0.0000001;
	else if(number > 0.0 && number < 0.0000001) return 0.0000001;
	else return number;
}
