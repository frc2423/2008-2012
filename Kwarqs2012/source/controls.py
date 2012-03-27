'''

This file defines the mapping of various controls here, so that way to change
way things are mapped, we don't have to change the actual logic or anything.

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
TRIGGER = wpilib.Joystick.kDefaultTriggerButton      # 1
TOP = wpilib.Joystick.kDefaultTopButton              # 2

# axis definitions -- a tuple of (stick number, axis)
# -> call GetJoystickAxis() with this value to get the axis

DRIVE_X_AXIS        = (1, X)
DRIVE_Y_AXIS        = (1, Y)
DRIVE_TRIGGER       = (1, TRIGGER)
MANUAL_CAMERA_AXIS  = (1, Z)

SUSAN_TURN_AXIS     = (2, X)
SHOOTER_WHEEL_AXIS  = (2, Z)


# button definitions -- (stick number, [list of buttons])
# -> call StickButtonPressed() with this value to get True/False

RAMP_ARM_DOWN_BUTTON = (1,6)
MANUAL_CAMERA_ENABLE_BUTTON1 = (1, TOP)
MANUAL_CAMERA_ENABLE_BUTTON2 = (1, 7)

SUSAN_TURN_BUTTON = (2, TOP)

BALL_HANDLER_MANUAL_CHAMBER_UP_BUTTON   = (2, 11)
BALL_HANDLER_MANUAL_CHAMBER_DOWN_BUTTON = (2, 10)
BALL_HANDLER_MANUAL_FEEDER_FEED_BUTTON  = (2, 6)
BALL_HANDLER_MANUAL_FEEDER_EXPEL_BUTTON = (2, 7)

SHOOT_BALL_BUTTON = (2, TRIGGER)

DEBUG_BUTTON1 = (1, 8)
DEBUG_BUTTON2 = (2, 8)


# switch definitions
# -> call GetSwitch() with this value to get True/False

# TODO: Fix these
MANUAL_SHOOTER_WHEEL_ENABLE_SWITCH  = 2
CONTINUOUS_BALL_HANDLER_SWITCH      = 4
AUTO_BALL_HANDLER_SWITCH            = 8
AUTO_SUSAN_SWITCH                   = 12
AUTO_DISTANCE_SWITCH                = 16

#
# Utility functions
#


ds = wpilib.DriverStation.GetInstance()
eio = ds.GetEnhancedIO()

# configure the switches
eio.SetDigitalConfig( MANUAL_SHOOTER_WHEEL_ENABLE_SWITCH, wpilib.DriverStationEnhancedIO.kInputPullUp )
eio.SetDigitalConfig( AUTO_BALL_HANDLER_SWITCH, wpilib.DriverStationEnhancedIO.kInputPullUp )
eio.SetDigitalConfig( CONTINUOUS_BALL_HANDLER_SWITCH, wpilib.DriverStationEnhancedIO.kInputPullUp )
eio.SetDigitalConfig( AUTO_SUSAN_SWITCH, wpilib.DriverStationEnhancedIO.kInputPullUp )
eio.SetDigitalConfig( AUTO_DISTANCE_SWITCH, wpilib.DriverStationEnhancedIO.kInputPullUp )


def GetJoystickAxis( tuple ):
    return ds.GetStickAxis( tuple[0], tuple[1] )

def StickButtonOn( tuple ):
    return ds.GetStickButtons( tuple[0] ) & (1 << (tuple[1]-1))

def SwitchOn( val ):
    return not eio.GetDigital( val )
 
