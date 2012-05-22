 



class BallHandler(object):
    '''
        This implements functions to automatically handle the balls on the
        robot. 
    '''

    def __init__(self, chamber, feeder, widget):
        
        self.chamber = chamber
        self.feeder = feeder
        
        self.widget = widget
        
        # ball tracking
        self.last_feeder_top = False
        self.last_feeder_low = False
        self.last_chamber_full = False
        
        self.feeder_virtual_ball = False
        
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
        pass
        
        #print("BallHandler: FLO: %s, FV: %s, FTOP: %s, CFUL: %s" % 
        #    (self.last_feeder_low, self.feeder_virtual_ball, self.last_feeder_top,
        #     self.last_chamber_full))
        #print("Voltage: Feeder: %.3f Chamber: %.3f" % (
        #    self.feeder.top_ball_sensor.irSensor.GetVoltage(),
        #    self.chamber.ball_sensor.irSensor.GetVoltage() ))
        
        
    def Update(self):
       
        auto_feed = self.auto_feed
        continuous_feed = self.continuous_feed
    
        #
        # set it up
        #
        
        # query ball sensors
        chamber_full =  bool(self.chamber.IsFull())
        feeder_top =    bool(self.feeder.HasTopBall())
        feeder_middle = bool(self.feeder.HasMidBall())
        feeder_low =    bool(self.feeder.HasLowBall())
        feeder_enter =  bool(self.feeder.HasEnterBall())
        
        # setup widget
        self.widget.SetAll( chamber_full, feeder_top, feeder_middle, feeder_low, feeder_enter )
        
        feeder_full = feeder_top and feeder_middle and feeder_low
        
        # automated feeding
    
        if auto_feed or continuous_feed:
            
            feed = False
            run_chamber = False
            
            # only feed when there are balls that can continue onwards
            # -> you can solve this using a truth table and a Karnaugh map
            # 
            #   E   L   M   T   |   F
            #  -----------------|-----
            #   -   -   -   -   |   -
            #   -   -   -   X   |   -
            #   -   -   X   -   |   X
            #   -   -   X   X   |   -
            #   -   X   -   -   |   X
            #   -   X   -   X   |   X
            #   -   X   X   -   |   X
            #   -   X   X   X   |   -
            #   X   -   -   -   |   X
            #   X   -   -   X   |   X
            #   X   -   X   -   |   X
            #   X   -   X   X   |   X
            #   X   X   -   -   |   X
            #   X   X   -   X   |   X
            #   X   X   X   -   |   X
            #   X   X   X   X   |   -
            #
            
            feed = (feeder_enter and not feeder_low) or \
                   (feeder_middle and not feeder_top) or \
                   (feeder_low and not feeder_middle)
            
            # enable the continuous feed functionality if required
            if continuous_feed and not feeder_full:
                feed = True
            
            # transfer the ball from the feeder to the chamber
            if feeder_top and not chamber_full:
                feed = True
                run_chamber = True
            
            # this is redundant
            #if chamber_full:
            #    run_chamber = False
            
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
    

        