'''
    Author: Who wrote this?
    Date: When was it written?
    
    Description:
    
        Put a useful description of your autonomous mode here
'''

try:
    import wpilib
    import wpilib.SmartDashboard
except ImportError:
    import fake_wpilib as wpilib
    import fake_wpilib.SmartDashboard


class TimerShooter(object):

    # this name should be descriptive and unique. This will be shown to the user
    # on the SmartDashboard
    MODE_NAME = "Timed Shooter"
    DEFAULT = True
    
    WHEEL_SPEED = 48.5


    def __init__(self, drive, ramp_arm, ball_handler, robot_manager):
        '''Constructor: store components locally here'''
        self.drive = drive
        self.ramp_arm = ramp_arm
        self.ball_handler = ball_handler
        self.robot_manager = robot_manager
        
        self.chamber = ball_handler.chamber
        self.feeder = ball_handler.feeder
        self.wheel = robot_manager.wheel
        
        self.sd = wpilib.SmartDashboard.SmartDashboard.GetInstance()
        
        self.turn_speed = 0.5
        self.forward_speed = 1.0
        
        self.sd.PutDouble("AUTO: Turn", self.turn_speed )
        self.sd.PutDouble("AUTO: Forward", self.forward_speed )
        self.sd.PutDouble("AUTO: Wheel Speed", self.WHEEL_SPEED )
        
    def OnEnable(self):
        '''
            This function is called when Autonomous mode is enabled. You should
            initialize things needed for your mode here
        '''
        
        self.turn_speed = self.sd.GetDouble("AUTO: Turn")
        self.forward_speed = self.sd.GetDouble("AUTO: Forward")
        self.WHEEL_SPEED = self.sd.GetDouble("AUTO: Wheel Speed")
        
        pass
        
    def OnDisable(self):
        '''
            This function is called when Autonomous mode is exiting. You should
            clean anything up here that needs to be cleaned up
        '''
        
        pass
        
    def Update(self, time_elapsed):
        '''
            This function is called every 10ms or so. This is where you should
            make decisions about what needs to happen in your autonomous mode.
            You do not need to call the 'Update' functions of any components
            here, as they will be called for you automatically.
            
            time_elapsed is the number of seconds that autonomous mode has been
            active, in case your mode finds that useful. 
        '''
        
        if   time_elapsed < 4.5:
            self.wheel.SetAutoSpeed( TimerShooter.WHEEL_SPEED )
            
        elif time_elapsed < 6.0:
            self.wheel.SetAutoSpeed( TimerShooter.WHEEL_SPEED )
            self.ramp_arm.Extend() # hehe
            self.chamber.Run()
            
        elif time_elapsed < 9.0: 
            self.wheel.SetAutoSpeed( TimerShooter.WHEEL_SPEED )
            
        elif time_elapsed < 9.5:
            self.wheel.SetAutoSpeed( TimerShooter.WHEEL_SPEED )
            self.feeder.Feed()
            self.chamber.Run()
            
        elif time_elapsed < 11.0:
            self.wheel.SetAutoSpeed( TimerShooter.WHEEL_SPEED )
            self.feeder.Feed()
            self.chamber.Run()
            self.ramp_arm.Extend()  # hehe
            
        # time to turn the robot around and go crazy!
        elif time_elapsed < 13.0:
            self.wheel.SetAutoSpeed( TimerShooter.WHEEL_SPEED )
            #self.drive.ArcadeDrive( -1.0, 0.0, False )
            #self.chamber.Run()
        
        #elif time_elapsed < 9.1:
        #    self.drive.ArcadeDrive( -0.1, 0.0, False )
        #    self.ramp_arm.Extend()
    
    
        else:
            # now get mad!
            #if int(time_elapsed % 2) == 1:
            #    self.ramp_arm.Extend()
            
            pass
        
