

class BallHandler(object):
    '''
        This implements functions to automatically handle the balls on the
        robot. 
    '''

  
    def __init__(self, chamber, feeder):
        
        self.chamber = chamber
        self.feeder = feeder
        
        self.disableAuto = True 
    

    


    
    
    def Update(self):
        '''
            Depending on where balls are, 
        '''    
    
    
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
        
        self.chamber.Update()
        self.feeder.Update()
