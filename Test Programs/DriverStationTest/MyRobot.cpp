#include "WPILib.h"
#include "DriverStationLCD.h"

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
		
		GetWatchdog().SetEnabled(true);
		while (IsOperatorControl())
		{
			GetWatchdog().Feed();
			
			if (GetTime() - tm > 0.1)
			{
				DriverStationLCD * lcd = DriverStationLCD::GetInstance();
				
				lcd->PrintfLine(DriverStationLCD::kMain_Line6, "Press select button");
				
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
						m_ds->GetAnalogIn(1));
				lcd->PrintfLine(DriverStationLCD::kUser_Line4, "2: %.1f", 
						m_ds->GetAnalogIn(2));
				lcd->PrintfLine(DriverStationLCD::kUser_Line5, "3: %.1f", 
						m_ds->GetAnalogIn(2));
				lcd->PrintfLine(DriverStationLCD::kUser_Line6, "4: %.1f", 
						m_ds->GetAnalogIn(2));
				
				lcd->UpdateLCD();
				
				tm = GetTime();
			}
		}
	}
};

START_ROBOT_CLASS(RobotDemo);

