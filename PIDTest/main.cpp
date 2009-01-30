
#include <stdio.h>

#include "Notifier.h"
#include "PIDDemo.h"

int main()
{
	float p = 0.5F, i = 0.0F, d = 0.0F;

	PIDDemo demo(p, i, d);	
	demo.SetPosition(345);
	demo.SetSetPoint(5);

	for (int i = 0; i < 40; i++)
	{
		Notifier::Pulse();
		printf("Output: %f %f: %s (err: %f, goal: %f)\n", 
			demo.GetOutput(), demo.GetPosition(),
			demo.m_controller.OnTarget() ? "on" : "off",
			demo.m_controller.GetError(),
			demo.m_controller.GetSetpoint());
	}		

	return 0;
}
