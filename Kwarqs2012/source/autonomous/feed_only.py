'''
    Author: Dustin Spicuzza
    Date:   3/22/2012
    
    Description:
    
        This mode only feeds another robot, does not move or anything
'''

class FeedOnlyAutonomousMode(object):

    # this name should be descriptive and unique. This will be shown to the user
    # on the SmartDashboard
    MODE_NAME = "Feed Only"
    DEFAULT = False


    def __init__(self, drive, ramp_arm, ball_handler, robot_manager):
        '''Constructor: store components locally here'''
        self.drive = drive
        self.ramp_arm = ramp_arm
        self.ball_handler = ball_handler
        self.robot_manager = robot_manager
        
    def OnEnable(self):
        '''
            This function is called when Autonomous mode is enabled. You should
            initialize things needed for your mode here
        '''
        
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
        
        self.ball_handler.chamber.Remove()
        self.ball_handler.feeder.Expel()
