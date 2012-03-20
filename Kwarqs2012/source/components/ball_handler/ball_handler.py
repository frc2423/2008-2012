 



class BallHandler(object):
    '''
        This implements functions to automatically handle the balls on the
        robot. 
    '''

    def __init__(self, chamber, feeder):
        
        self.chamber = chamber
        self.feeder = feeder
        
        # ball tracking
        self.last_feeder_top = False
        self.last_feeder_low = False
        self.last_chamber_full = False
        
        self.feeder_virtual_ball = False
        self.middle_virtual_ball = False
        
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
        
    def Print(self):
        print("BallHandler: FLO: %s, FV: %s, FTOP: %s, VM: %s, CFUL: %s" % 
            (self.last_feeder_low, self.feeder_virtual_ball, self.last_feeder_top,
             self.middle_virtual_ball, self.last_chamber_full))
        
    #
    # Internal logic for ball tracking
    #
    
    def _calculate_virtual_balls(self, feeder_top, feeder_low, chamber_full):
        ''' 
            Internal function: virtual ball tracking
        
            This is important because we need to ensure that
            when a ball is in the feeder or chamber, that we don't
            all of a sudden stop if the user lets go of the button,
            and continue the ball to the top of the robot     

            A ball goes into the 'virtual' state when the sensors
            can no longer see the ball, but logically it must
            be there because the belt is going the right way.
        '''
    
        # note that we set the 'True' state last for each case, 
        # because I'd rather think there was a ball there,
        # as opposed to ignoring the ball
        
        # 1 is up, -1 is down, 0 is stopped
        UP = 1
        DOWN = -1
        
        feeder_dir = self.feeder.GetDirection() 
        chamber_dir = self.chamber.GetDirection()
    
    
        # virtual feeder ball
        
        if feeder_dir == UP:
        
            if feeder_top and not self.last_feeder_top:
                self.feeder_virtual_ball = False

            if not feeder_low and self.last_feeder_low:
                self.feeder_virtual_ball = True
        
        elif feeder_dir == DOWN:
            
            if feeder_low and not self.last_feeder_low:
                self.feeder_virtual_ball = False
        
            if not feeder_top and self.last_feeder_top:
                self.feeder_virtual_ball = True
    
    
        # virtual middle ball
    
        if chamber_dir == UP and feeder_dir >= 0:
                
            if chamber_full and not self.last_chamber_full:
                self.middle_virtual_ball = False
    
            if not feeder_top and self.last_feeder_top:
                self.middle_virtual_ball = True
        
        elif chamber_dir == DOWN and feeder_dir <= 0:
            
            if feeder_top and not self.last_feeder_top:
                self.middle_virtual_ball = False
                
            if not chamber_full and self.last_chamber_full:
                self.middle_virtual_ball = True
        
        
        self.last_feeder_top = feeder_top
        self.last_feeder_low = feeder_low
        self.last_chamber_full = chamber_full
    
        
        
    def Update(self):
       
        auto_feed = self.auto_feed
        continuous_feed = self.continuous_feed
    
        #
        # set it up
        #
        
        # query ball sensors
        chamber_full = self.chamber.IsFull()
        feeder_top = self.feeder.HasTopBall()
        feeder_low = self.feeder.HasLowBall()
        
        # perform ball tracking
        self._calculate_virtual_balls( feeder_top, feeder_low, chamber_full )
        
        feeder_full = feeder_top and feeder_low
        has_ball = feeder_top or feeder_low or self.feeder_virtual_ball or self.middle_virtual_ball
        
        # automated feeding
    
        if auto_feed:
            
            feed = False
            run_chamber = False
            
            
            if (continuous_feed or has_ball) and not feeder_full:
                feed = True
            
            
            if (feeder_top or self.middle_virtual_ball) and not chamber_full:
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
        
        # reset vars
        self.auto_feed = False
        self.continuous_feed = False
    

        