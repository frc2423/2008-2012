
#include <stdio.h>
#include "CompassDrive.h"

int main()
{
	CompassDrive myDrive;
	
	for (int i = -20; i < 20; i++)
	{
		float heading = i * 22.5;
		printf("heading: %f degrees\n", heading);

		// some loop here to generate x and y values for the joystick
		
		for (float x = -1; x < 1; x += 0.1)
		{
			for (float y = -1; y < 1; y += 0.1)
			{
				for (float x2y = -1; x2y < 1; x2y += 0.1)
				{
					printf("\tX: %f, Y: %f, ", x, y);
					myDrive.Drive(heading, x, y, x2y);
				}
			}
		}
	}
	
}


	
	
