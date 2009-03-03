#include "WPILib.h"
#include "DriverStationLCD.h"

#include "filters.h"
#include <cmath>

class RobotDemo : public SimpleRobot
{
public:
	RobotDemo(void)
	{
		GetWatchdog().SetExpiration(0.1);
	}


	void OperatorControl(void)
	{
		double tm = GetTime();
		
		AverageWindowFilter<double, 20> filter1;
		AverageWindowFilter<double, 20> filter2;
		AverageWindowFilter<double, 20> filter3;
		AverageWindowFilter<double, 20> filter4;
		
		GetWatchdog().SetEnabled(true);
		while (IsOperatorControl())
		{
			GetWatchdog().Feed();
			
			if (GetTime() - tm > 0.1)
			{
				DriverStationLCD * lcd = DriverStationLCD::GetInstance();
				
				lcd->PrintfLine(DriverStationLCD::kUser_Line2, 
						"%d %d %d %d %d %d %d %d",
						(int)m_ds->GetDigitalIn(1),
						(int)m_ds->GetDigitalIn(2),
						(int)m_ds->GetDigitalIn(3),
						(int)m_ds->GetDigitalIn(4),
						(int)m_ds->GetDigitalIn(5),
						(int)m_ds->GetDigitalIn(6),
						(int)m_ds->GetDigitalIn(7),
						(int)m_ds->GetDigitalIn(8)
				);
				
				const double c = 500.0 / 1000.0;
				
				filter1.AddPoint( ceil( m_ds->GetAnalogIn(1) * c));
				filter2.AddPoint( ceil( m_ds->GetAnalogIn(2) * c));
				filter3.AddPoint( ceil( m_ds->GetAnalogIn(3) * c));
				filter4.AddPoint( ceil( m_ds->GetAnalogIn(4) * c));
				
								
				lcd->PrintfLine(DriverStationLCD::kUser_Line3, "1: %.1f", 
						filter1.GetAverage());
				lcd->PrintfLine(DriverStationLCD::kUser_Line4, "2: %.1f", 
						filter2.GetAverage());
				lcd->PrintfLine(DriverStationLCD::kUser_Line5, "3: %.1f", 
						filter3.GetAverage());
				lcd->PrintfLine(DriverStationLCD::kUser_Line6, "4: %.1f", 
						filter4.GetAverage());
				
				lcd->UpdateLCD();
				
				tm = GetTime();
			}
		}
	}
};

START_ROBOT_CLASS(RobotDemo);

