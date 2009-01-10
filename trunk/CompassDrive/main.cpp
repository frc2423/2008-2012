
#include <stdio.h>
#include "CompassDrive.h"

int main()
{
	CompassDrive myDrive;
	
	for (int i = -5; i < 5; i++)
	{
		float heading = i * 80;
		printf("robot_Compass: %.2f degrees\n", heading);

		// some loop here to generate x and y values for the joystick
		
		for (float x = -1.0; x < 1.1; x += 0.5)
		{
			for (float y = -1.0; y < 1.1; y += 0.5)
			{
				for (float x2y = -1.0; x2y < 1.1; x2y += 0.5)
				{
					printf("\tX: %.3f, Y: %.3f, Speed: %.3f ", x, y, x2y);
					myDrive.Drive(heading, x, y, x2y);
				}
			}
		}
	}
	
}


	
	
