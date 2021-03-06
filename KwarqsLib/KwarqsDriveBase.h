/**
	\file 		KwarqsDriveBase.h
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/


#ifndef KWARQSLIB_KWARQS_DRIVEBASE_H
#define KWARQSLIB_KWARQS_DRIVEBASE_H

/**
	\class KwarqsDriveBase
	\brief The base class used to move the robot around in specified directions

	No classes directly interact with this class, they interact with the
	KwarqsDriveController which calls these classes. There will be a number 
	of objects that will inherit from this, and different types of classes
	can be implemented. 
	
	The ultimate goal for this is for a number of different classes to
	be able to modify the speed/direction of the bot, without having to
	modify any of the other classes. 
*/
class KwarqsDriveBase {
public:

	/// \name Methods that derived classes must implement
	/// @{
	
	/// Called when this drive is disabled
	virtual void Disable() = 0;
	
	/// Called when this drive is enabled
	virtual void Enable() = 0;

	/// @return true if this class physically talks to the motors to make the
	/// robot move. False if this is a filter or other such drive type
	virtual bool IsPhysicalDrive() = 0;
	
	/**
		\brief Performs a movement of the robot
		
		The KwarqsDriveController calls this method on every enabled drive
		type when Move() is called on it. 
		
		Both parameters are passed by reference, so they may be modified
		by filter drive types when this is called.
		
		@param speed	Speed of the robot (-1 to 1)
		@param angle	Angle for the robot to move (0 to 360)
		@param rotation	Rotational speed of the robot (-1 to 1)
		@param stop		If true, then stop the robot absolutely
	*/
	virtual void Move(double &speed, double &angle, double &rotation, bool &stop) = 0;

	/// Return the name of the class
	virtual const char * Name() = 0;
	
	/// @}
	
	// virtual destructor
	virtual ~KwarqsDriveBase(){}
};

#endif
