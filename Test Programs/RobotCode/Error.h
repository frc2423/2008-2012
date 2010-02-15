#include <WPILib.h>

//Function for correcting divide by 0 run time error

float d_err(float number)
{
	if(number < 0.0 && number > -0.000001) return -0.0000001;
	else if(number > 0.0 && number < 0.0000001) return 0.0000001;
	else return number;
}
