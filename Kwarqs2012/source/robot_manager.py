try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib


class RobotManager(object):
    '''
        This currently implements automated feeding functionality
        for the robot, and possibly will implement automated
        shooting as well. Probably.
        
        TODO: Perhaps we need a hybrid mode? Where the human tells the 
        thing when to feed, but once a ball is detected it automatically
        puts the ball into the chamber? Or something similar.
    '''

  
    def __init__(self, chamber, feeder, wheel, susan, vAngle, shooter):
        
        
        self.chamber = chamber
     
        self.feeder = feeder
        
        self.wheel = wheel
        self.susan = susan
        self.vAngle = vAngle
        self.shooter = shooter
        
        self.disableAuto = True 
    

    '''
    Calls the shoot function if the bot is ready to shoot
    '''
    def ShootIfReady(self):
        if self.shooter.IsReady() and self.chamber.IsReady():
            self.chamber.Run()
        
    def DisableAutoFeeder(self):
        self.disableAuto = True


    '''
    Calls functions within the components based on every possible state
    '''    
    
    def Update(self):

        # automated feeding
    
        if not self.disableAuto:
        
            #
            # set it up
            #
        
            feeder_has_top_ball = self.feeder.HasTopBall()
            feeder_full = self.feeder.IsFull()
            chamber_full = self.chamber.IsFull()
            is_shooting = self.shooter.IsShooting()
            
            
            feed = False
            run_chamber = False
            
            
            if not feeder_full:
                # always do this when we're in auto mode
                feed = True
            
            
            if feeder_has_top_ball and not chamber_full:
                feed = True
                run_chamber = True
            
            if is_shooting:
                run_chamber = True
            elif chamber_full:
                run_chamber = False
            
            #
            # Do it
            #
            
            if feed:
                self.feeder.Feed()
            else:
                self.feeder.Stop()
                
            if run_chamber:
                self.chamber.Run()
            else:
                self.chamber.Stop()
            

        self.disableAuto = False
        
        #
        # Update phase
        #
        
        self.shooter.Update()
        self.chamber.Update()
        self.feeder.Update()
