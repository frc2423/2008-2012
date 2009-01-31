
#include <math.h>
#include <stdio.h>

int main()
{
	for (double y = 1.0; y > -1.1; y -= 0.25)
	{
		for (double x = -1.0; x < 1.1; x += 0.25)
		{

			double angle = (atan2(y, x) * (180/M_PI) - 90.0 ) * -1;
			
			if (angle < 0) angle += 360;
			//angle -= 90.0;
			
			printf("%4.0f ", angle);
		}
		
		printf("\n");
	}
}

