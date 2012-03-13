
 

 
class RobotManager(object):
    '''
        The robot has a lot of moving parts, and as such it would be really
        tedious if the drivers had to think about everything that was happening
        and make sure that they happen. 
        
        This module tries to coordinate all the individual parts of the
        robot and enhance the driver experience by implementing automated 
        behaviors. This enhances the driver experience. 
    '''

 
    def __init__(self, ball_handler, camera, wheel, susan, console_led):

        self.ball_handler = ball_handler
        self.camera = camera
        self.wheel = wheel
        self.susan = susan
        self.console_led = console_led
        
        
    def EnableAutoSusan(self):
        pass
        
    def EnableAutoDistance(self):
        pass
        
    def ShootBall(self):
        # once everything is ready, then this causes the chamber to run no matter what
        # for at least some N seconds so we ensure the ball gets out
        pass
        
    def Print(self):
        # TODO
        pass
    
    def Update(self):
        
        pass
        
        