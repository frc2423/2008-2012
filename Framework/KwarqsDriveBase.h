

/**
	\class KwarqsDriveBase
	\brief The base class used to move the robot around in specified directions

	This is just the abstract base specification -- it is anticipated that there
	will be multiple classes with this, that can perform layered/chained 
	modifications to the robot's movement. Layering will be ideally be 
	transparent and implemented inside of the base class. Proposed classes 
	include:
	
	KwarqsDrive
		This class actually interacts with the wheels and motors (via 
		KwarqsWheelMotor and KwarqsWheelServoBase objects) to make the
		robot move. We may make more than one of these. 
		
	KwarqsDriveCompensation
		This class adjusts the incoming speed/direction from the user/other 
		according to the drift or other error conditions of the robot movement,
		and passes it to a lower level KwarqsDriveBase object
		
		Open question: is this the only type of compensation that would be 
		useful to have? Or should it be more fine grained (ie, have access 
		to individual motors and get their error). I think a more coarse
		approach is better, but maybe not. 
		
	KwarqsDriveLogger
		This class takes incoming movements, and logs them to a file for later
		analysis. It calls a lower KwarqsDrive 
		
	@note Layering functions are not shown below at the moment, but its pretty
	simple to acheive
*/
class KwarqsDriveBase {
public:

	/// \name Methods that derived classes must implement
	/// @{

	/// Performs a movement of the robot. The rotation parameter defaults
	/// to zero by default. 
	virtual void Move(float speed, float heading, float rotation = 0) = 0;

	/// Stops the robot dead in its tracks (is this useful? not sure)
	virtual void Stop() = 0;
	
	/// Return the name of the class
	const char * Name() = 0;
	
	/// @}
};
