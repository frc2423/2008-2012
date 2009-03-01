
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char ** argv)
{
	if (argc < 2)
		return 0;

	double angle = strtod(argv[1], 0);

	for (double current_angle = 0; current_angle < 361; current_angle += 15)
	{
		double alternate_angle = angle + 180 > 360 ? angle - 180 : angle + 180;

		double error1 = angle - current_angle;
		double error2 = alternate_angle - current_angle;

		if (fabs(error1) > 180)
		{
			if (error1 < 0)
				error1 += 360;
			else
				error1 -= 360;
		}

		if (fabs(error1) < fabs(error2))
			printf("%.1f: %f (%f < %f)\n", current_angle, angle, error1, error2);
		else
			printf("%.1f: %f (%f > %f)\n", current_angle, alternate_angle, error1, error2);
	}
	
	return 0;
}
	
