'''
    Author: Who wrote this?
    Date: When was it written?
    
    Description:
    
        This autonomous mode is intended to expel all held balls (to a team 
		mate). After this, the robot will head to a (*be more specific*) ramp, 
		where it will use its ramp arm to knock the balls off the ramp.
		
		This mode should only be used when we have a team mate who clearly
		has a better shooting autonomous mode than us. 
''' 

from . import AutonomousModeBase



class MyAutonomousMode(AutonomousModeBase):

    # this name should be descriptive and unique. This will be shown to the user
    # on the SmartDashboard
    NAME = "Feed Teammates"


    def __init__(self, drive, ramp_arm, ball_handler, shooter, robot_manager):
        '''Constructor: store components locally here'''
        self.drive = drive
        self.ramp_arm = ramp_arm
        self.ball_handler = ball_handler
        self.shooter = shooter
        self.robot_manager = robot_manager
        
		#This variable will store the current time when all balls have been expelled
		self.basetime = 0
		
		#This variable is a state variable. I only want to store self.basetime once
		#(when the ball manager is first empty)
		self.x = 0
		
		
    def OnEnable(self):
        '''
            This function is called when Autonomous mode is enabled. You should
            initialize things needed for your mode here
        '''
        self.ball_handler.Feed()
        
        
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
		
		#This function name will need to be changed. I simply guessed as to
		#what it would be called
		
        if ball_handler.IsEmpty() and self.x == 0:
			self.basetime = time_elapsed
			self.x = 1
		
		if 	time_elapsed == self.basetime + 1:
			#tell robot to move to the ramp