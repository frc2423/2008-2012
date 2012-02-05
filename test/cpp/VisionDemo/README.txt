README for FRC Vision Demo

This is based on the SimpleDemo program, with changes in autonomous to react to colors seen by the camera.

RUNNING:
It depends on both the frcvision and WPILib libraries to compile and run. 
The camera is started and initialized in the constructor.

If you have a serial output to monitor the console, the current status will be printed.


OPTIONS:
Several options are coded; you can switch between them by changing the variable "va" to the following values:

STOPLIGHT
   The default value
   Drives forward when it sees green
   Drives reverse when it sees red
   Otherwise no movement

GO_FORWARD
   Drives forward (at 1/5 full speed) when it sees green
   Otherwise no movement

FOLLOW_COLOR
   Follows the red target when it sees red
   Otherwise no movement

MOVE_AWAY
   Drives reverse when it sees purple
   Otherwise no movement

SNAPSHOT
   Takes a picture, stores it in a file on the cRIO and then stops


Forward speed can be change by updating "mySpeed"

Colors and lighting conditions can be altered by selecting any of the predefined colors in 
TrackAPI.cpp::GetTrackingData, or by creating your own ranges of hue, saturation and luminance values.


The NI Vision Assistant is useful for color thresholding to determine what
values work best with a particular target.

Documentation for the vision API is in the FRC Vision Specification on Sourceforge
(doc6166).

Documentation for the camera API and various utilites used by the API is in the 
C/C++ Programming Guide on Sourceforge (doc6119).

TROUBLESHOOTING:
Verify that camera is set up correctly - can you ping "192.168.0.90" from the cRIO? 
When you boot the cRIO, do you see an IP address for its 2nd port to talk to the camera?

WARNING:
If you stand in front of the robot waving a green object be ready to run!!!!