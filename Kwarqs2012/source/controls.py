'''

This file defines the mapping of various controls here, so that way to change
way things are mapped, we don't have to change the actual logic or anything.


Stick1:

    X/Y axis drives robot
    Trigger: smoother turning
    
    6/11: Ramp arm down
    
    Z axis: manual camera angle
    Top button: enable manual camera pointing
    
Stick2:

    Shooter:
        Automated:
            Trigger: fire ball (move ball from chamber)
            
        Manual:
            Trigger: activate wheel
            Z axis: manual wheel speed
        
            6: move ball from chamber into shooter
            
    
    Susan:
        Automated:
            None
    
        Manual:
            Top button: activate manual lazy susan
            X axis: turn lazy susan
                
            
    Ball Handling:
        Continuous feeding:
            None
    
        Automated:
            10/11: activate feeding
        
        Manual:
            6/7: manual chamber 
            10/11: manual feeder
    
        
Digital inputs (switches):
    
    Auto distance enable/disable
    Auto susan enable/disable
    Auto shoot enable/disable
    Auto ball handling enable/disable
    Continuous feeding enable/disable
        -> perhaps this should be on the dashboard?


'''


try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib

# axis constants
X = wpilib.Joystick.kDefaultXAxis
Y = wpilib.Joystick.kDefaultYAxis
Z = wpilib.Joystick.kDefaultZAxis

# button constants
TRIGGER = wpilib.kDefaultTriggerButton      # 1
TOP = wpilib.kDefaultTopButton              # 2

# axis definitions -- a tuple of (stick number, axis)
# -> call GetJoystickAxis() with this value to get the axis

DRIVE_X_AXIS = (1, X)
DRIVE_Y_AXIS = (1, Y)

MANUAL_SUSAN_SPEED_AXIS = (2, X)
MANUAL_WHEEL_SPEED_AXIS = (2, Z)


# button definitions -- (stick number, [list of buttons])
# -> call StickButtonPressed() with this value to get True/False

AUTOMATIC_SHOOTER_FIRE_BUTTON   = (2, TRIGGER)
MANUAL_SHOOTER_WHEEL_BUTTON     = (2, TRIGGER)
MANUAL_SHOOTER


# switch definitions
# -> call GetSwitch() with this value to get True/False

AUTO_DISTANCE_SWITCH

#
# Utility functions
#


ds = wpilib.DriverStation.GetInstance()


def GetJoystickAxis( tuple ):
    return ds.GetStickAxis( tuple[0], tuple[1] )

def StickButtonPressed( tuple ):
    for 
    return ds.GetStickButtons

def SwitchOn( val ):
    return 
 
