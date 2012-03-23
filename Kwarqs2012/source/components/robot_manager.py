
try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib

 
class RobotManager(object):
    '''
        The robot has a lot of moving parts, and as such it would be really
        tedious if the drivers had to think about everything that was happening
        and make sure that they happen. 
        
        This module tries to coordinate all the individual parts of the
        robot and enhance the driver experience by implementing automated 
        behaviors. This enhances the driver experience. 
    '''

    # the amount of time the chamber is run to shoot something
    SHOOTING_PERIOD = 1.5
    
    # we check the shooter to determine whether or not it thinks
    # the ball was shot. However, if it says it was shot before a 
    # certain amount of time, we shouldn't believe it
    MIN_SHOOTING_PERIOD = 0.5
    
    BLINK_PERIOD_READY = 1.0
    BLINK_PERIOD_SHOOTING = 0.3
    

    
    def __init__(self, ball_handler, camera, wheel, susan, console_led):

        self.ball_handler = ball_handler
        self.camera = camera
        self.wheel = wheel
        self.susan = susan
        self.console_led = console_led
        
        self.ds = wpilib.DriverStation.GetInstance()
        
        self.auto_susan = False
        self.auto_distance = False
        self.shoot_ball = False
        
        # shooting state machine
        self.shoot_timer = wpilib.Timer()
        self.shooting = False
        self.was_shot = False
        
        
    def EnableAutoSusan(self):
        self.auto_susan = True
        
    def EnableAutoDistance(self):
        self.auto_distance = True
        
    def ShootBall(self):
        # once everything is ready, then this causes the chamber to run no matter what
        # for at least some N seconds so we ensure the ball gets out
        self.shoot_ball = True
        
    def SetUserWheelSpeed(self, speed):
        '''This is the speed the user wants the thing going to if auto distance 
           is not enabled AND they hit the fire button'''
        self.user_speed = speed
        
    def Print(self):
        print( "RobotManager: Shooting: %s" % self.shooting )
    
    #def Turn(self, speed):
    #    l_driveMotor.Set( speed )
    #    r_driveMotor.Set( -speed )
    
    def Update(self):
    
        # problems to deal with here:
        #   - Determining if the wheel was shot
        #   - Determining how long to shoot
        
        # make sure we don't allow bugs to happen
        if not self.ds.IsFMSAttached():
            if self.shoot_ball and self.auto_distance is None and self.user_speed is None:
                # .. is this going to bite us during autonomous?
                raise RuntimeError("ERROR: Must call one of these if you call ShootBall" )
        
        
        # get the tracking data from the camera object
        
        
        # speed the robot should be shooting according to the
        # automated system
        if self.auto_distance:
            speed = self.camera.TrackingData.distance
        elif self.user_speed is not None:
            speed = self.user_speed
    
        # always maintain the current susan angle when auto is enabled
        if self.auto_susan and not self.susan.IsSet():
            self.susan.SetAutoRelativeAngle( self.camera.TrackingData.angle_susan )
            
        ball_ready = self.ball_handler.IsReadyToShoot()
        wheel_ready = self.wheel.IsReady()
        camera_ready = self.camera.IsReadyToShoot()
        susan_ready = self.susan.IsReady()
    
        if not self.shooting and self.shoot_ball:
        
            # user wants to shoot the ball, so make sure that the wheel is
            # spinning an appropriate speed
            
            # always let the user override
            if not self.wheel.IsSet():                    
                self.wheel.SetAutoSpeed( speed )
            
        
            # only shoot when things are ready
            # -> we do not wait for the values to stabalize here. Presume that if stability
            #    calculations need to be made, then it is already done at the lower level
            if ball_ready and camera_ready and wheel_ready and susan_ready:
            
                # cache all values here so nothing bad happens
            
                self.shooting = True
                self.was_shot = False
                self.shoot_timer.Reset()
                self.shoot_timer.Start()
        
        if self.shooting:
        
            # TODO: Decide when to blink the LED and when to keep the light steady
            #self.console_led.Blink(
            
            # TODO: Operator LED for showing speeds.. 
                        
            shot = False
            
            # maintain the current wheel speed
            # TODO
            
            # we don't set the susan angle here, we set it elsewhere. This
            # is because it should be constantly adjusting as new data comes in
            
            
            # the wheel should be able to determine once a ball gets stuck in
            # it, since the speed should lower dramatically
            if self.wheel.DetectedShootEvent():
                self.was_shot = True
            
            if self.timer.Get() > RobotManager.MIN_SHOOTING_PERIOD:
                if self.was_shot:
                    shot = True
            
            if self.shoot_timer.HasPeriodPassed( RobotManager.SHOOTING_PERIOD ):
                shot = True
            
            if shot:
                # we're done shooting, don't do anything
                self.shooting = False
            else:
                # here I think we have to ignore the user's request, and move the
                # chamber regardless, since this only gets called when they request it
                self.ball_handler.ShootBall()
                
        
        else:
            if camera_ready and susan_ready:
                self.console_led.Blink( RobotManager.BLINK_PERIOD_READY )
       
        
        # thought: when setting the susan automatically from tracking data, 
        # it probably makes sense to decay the value over time, on the off chance
        # that there is some unwanted latency in the connection.
        
        # the decay value should be proportional to the value being sent to the jaguar
        
        # we should also make this really easy to enable/disable, because it might not 
        # work as well as one might hope
        
        # also, if the user interrupts the auto susan, then we should not set a value until
        # the next time that we get tracking data
                
        
        # reset vars
        self.auto_susan = False
        self.auto_distance = False
        self.shoot_ball = False
        
        