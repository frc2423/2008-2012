
/**
	\class KwarqsMovementControl
	\brief Base class for anything that interacts with the user to move the robot
	
	This class is used exclusively for driving, and does not deal with moving 
	the	game pieces.
	
	Input: If the derived class needs to interact with a joystick or other
	input device, it should create the appropriate objects and interact with 
	them. Special note: joysticks should be created using the static function
	Joystick::GetStickForPort(), so that Joystick objects are more easily shared 
	between classes. 
	
	Output: This should only use the KwarqsDriveBase object to move the robot.
*/
class KwarqsMovementControl {
public:

	/**
		Constructor. Derived classes may have other parameters passed to them
		if needed. 
	*/
	KwarqsMovementControl( KwarqsDriveBase * driveObject );
	
	
	/// \name Methods that need to be overriden by derived classes
	/// @{
	
	/// This is called repeatedly in OperatorControl mode, this method
	/// should use the m_drive object to move the Robot
	virtual void Move() = 0;
	
	/// This returns the name of the class (useful for generic logging)
	virtual const char * Name() = 0;
	
	/// @}
	
	
	/// \name Utility methods - inherited by derived classes
	/// @{
	
	/// This is called when a new drive object is needed (for example, a filter
	/// class or something to that effect)
	void SetDriveObject(KwarqsDrive * driveObject);
	
	/// @}
	
protected:

	//! This variable is used to move the robot around. Assume it is not NULL
	KwarqsDriveBase * m_driveObject;
	
};
