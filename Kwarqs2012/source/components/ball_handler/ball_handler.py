 


class BallHandler(object):
    '''
        This implements functions to automatically handle the balls on the
        robot. 
    '''

    def __init__(self, chamber, feeder):
        
        self.chamber = chamber
        self.feeder = feeder
        
        # ball tracking state
        self.feeder_last_bottom = False
        self.feeder_has_middle = False          # true if a ball is traveling between sensors
        self.feeder_last_top = False
        self.has_inbetween = False              # true if a ball is traveling between feeder/chamber
        self.chamber_last_full = False
        
        # state
        self.auto_feed = False
        self.continuous_feed = False
    
    def AutoFeed(self):
        '''Call this to enable auto feeding functionality'''
        self.auto_feed = True
    
    def ContinuousFeed(self):
        '''Call this to enable keeping the feeder on until the feeder is full'''
        self.continuous_feed = True
        
    def IsReadyToShoot(self):
        '''Returns True if there is a ball in position ready to shoot'''
        return self.chamber.IsFull()
        
    def ShootBall(self):
        '''Call this to cause the ball to be shot'''
        self.chamber.Run()
        
        
    def Update(self):
        '''
            Depending on where balls are, 
        '''    
    
        auto_feed = self.auto_feed
        continuous_feed = self.continuous_feed
    
        #
        # set it up
        #
        
        feeder_has_top_ball = self.feeder.HasTopBall()
        feeder_has_bottom_ball = self.feeder.HasBottomBall()
        feeder_full = self.feeder.IsFull()
        chamber_full = self.chamber.IsFull()
    
    
        # automated feeding
    
        if auto_feed:
        
            feed = False
            run_chamber = False
            
            
            if continuous_feed and not feeder_full:
                feed = True
            
            
            if feeder_has_top_ball and not chamber_full:
                feed = True
                run_chamber = True
            
            if chamber_full:
                run_chamber = False
            
            #
            # Do it (unless the user already did something)
            #
            
            if not self.feeder.IsSet():
                if feed:
                    self.feeder.Feed()
                else:
                    self.feeder.Stop()
            
            if not self.chamber.IsSet():
                if run_chamber:
                    self.chamber.Run()
                else:
                    self.chamber.Stop()

        # track balls traveling between sensors
        # -> this is important because we need to ensure that
        # when a ball is in the feeder or chamber, that we don't
        # all of a sudden stop if the user lets go of the button,
        # and continue the ball to the top of the robot
                    
        # TODO: implement this logic
                    
        # reset vars
        self.auto_feed = False
        self.continuous_feed = False
        
