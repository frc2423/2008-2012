
#include <stdio.h>

#include "Notifier.h"
#include "PIDDemo.h"

int main()
{
	float p = 0.5F, i = 0.0F, d = 0.0F;

	PIDDemo demo(p, i, d);	
	demo.SetPosition(140);
	demo.SetSetPoint(355);

	for (int i = 0; i < 40; i++)
	{
		if (demo.OnTarget())
			break;

		printf("%2d: Output: %.3f %.3f: %s (err: %.3f, goal: %.3f)\n",
			i,
			demo.GetOutput(), demo.GetPosition(),
			demo.m_controller.OnTarget() ? "on" : "off",
			demo.m_controller.GetError(),
			demo.m_controller.GetSetpoint());

		Notifier::Pulse();

		
	}		

	return 0;
}
