#include "WPILib.h"
#include "DriverStationLCD.h"

#include "filters.h"


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
		
		LowPassFilter filter1;
		LowPassFilter filter2;
		LowPassFilter filter3;
		LowPassFilter filter4;
		
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
								
				lcd->PrintfLine(DriverStationLCD::kUser_Line3, "1: %.1f", 
						filter1.Calculate( m_ds->GetAnalogIn(1)) );
				lcd->PrintfLine(DriverStationLCD::kUser_Line4, "2: %.1f", 
						filter2.Calculate( m_ds->GetAnalogIn(2)) );
				lcd->PrintfLine(DriverStationLCD::kUser_Line5, "3: %.1f", 
						filter3.Calculate( m_ds->GetAnalogIn(3)) );
				lcd->PrintfLine(DriverStationLCD::kUser_Line6, "4: %.1f", 
						filter4.Calculate( m_ds->GetAnalogIn(4)) );
				
				lcd->UpdateLCD();
				
				tm = GetTime();
			}
		}
	}
};

START_ROBOT_CLASS(RobotDemo);

