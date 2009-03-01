
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char ** argv)
{
	if (argc < 2)
		return 0;

	double setpoint = strtod(argv[1], 0);
	double m_result = 0;

	for (double angle = 0; angle < 361; angle += 15)
	{
			// get the difference between the angles
			double error = setpoint - angle;
			
			// find the shortest rotation distance
			if (fabs(error) > 180)
			{
				if (error > 0)
					error -= 360;
				else
					error += 360;
			}
			
			// any error under 1 degree is ok by me, stop rotating
			if (fabs(error) < 1.0)
				m_result = 0;
			else
			{
				// scale the rotation parameter according to how close we currently are		
				
				// linear:
				//m_result = (error * .5) / 180;
				
				// sqrt:
				if (error < 0)
					m_result = ((sqrt(fabs(error)) * .5) / sqrt(190))*-1;
				else
					m_result = (sqrt(error) * .5) / sqrt(190);
			}

			printf("%.1f: %f %f\n", angle, error, m_result);
	}
	
	return 0;
}
	
