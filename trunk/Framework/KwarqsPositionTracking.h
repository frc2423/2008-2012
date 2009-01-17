
/**
	\class KwarqsPositionTracking
	\brief Provides methods to get the position of the robot
	
	There will be a single instance of this class, and pointers of this 
	should be passed in to other classes that need position information. 
	
	Open question: how can this integrate wheel information to compensate
	for errors? Possibly a Set() function of some kind.. 
*/
class KwarqsPositionTracking {
public:

	/// returns the position of the bot on the field in X/Y coordinates
	void GetPosition(float &x, float &y);
	
	/// returns the heading of the robot
	float GetHeading();
	
	/// returns the acceration of the robot
	void GetAcceleration(float &x, float &y);
	
	/// returns the velocity of the robot
	void GetVelocity(float &x, float &y);
};
