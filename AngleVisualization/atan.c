
#include <math.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char ** argv)
{
	double current_angle = argc < 2 ? 0 : strtol(argv[1], NULL, 10);

	for (current_angle = 0; current_angle < 361.0; current_angle += 15.0)
	{
		printf("\nAngle: %f\n", current_angle);
	
		for (double y = 1.0; y > -1.1; y -= 0.25)
		{
			for (double x = -1.0; x < 1.1; x += 0.25)
			{

				double desired_angle = (atan2(y, x) * (180/M_PI) - 90.0 ) * -1;			
				if (desired_angle < 0) desired_angle += 360;

				

				double rotateValue = (desired_angle - current_angle );
					
				if (rotateValue > 180)
					rotateValue = rotateValue - 360.0;
				else if (rotateValue < -180)
					rotateValue = rotateValue + 360.0;
					
				if (fabs(rotateValue) < 15.0 )
					rotateValue = 0;
				
				rotateValue = rotateValue / 90.0;
				if (rotateValue > 1.0)
					rotateValue = 1.0;
				else if (rotateValue < -1.0)
					rotateValue = -1.0;
				
				printf("%4.4f ", rotateValue);
			}
			
			printf("\n");
		}
	}
}

