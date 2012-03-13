
try:
    import wpilib
    import wpilib.SmartDashboard
except ImportError:
    import fake_wpilib as wpilib
    import fake_wpilib.SmartDashboard
    
    
    
class Camera(object):
    '''
        The robot has a camera perched on the top of the robot with an
        LED ring on it. The camera also is mounted on a servo, so we can 
        point it up and down. We do vision processing on the driver 
        station with the SmartDashboard software. 
        
        This class receives information from the SmartDashboard and
        stores it, and also controls the servo to point the camera.
    '''

    # if nobody tells the camera where to point, point it here
    DEFAULT_CAMERA_ANGLE = 180.0
    
    LED_RELAY_ON    = wpilib.Relay.kForward 
    LED_RELAY_OFF   = wpilib.Relay.kOff
    
    
    def __init__(self, camera_servo_ch, led_relay_ch):
        
        self.servo = wpilib.Servo(camera_servo_ch)
        self.camera_angle = None
        
        # this never changes, set it and forget it
        # -> TODO: Should we turn this off to conserve power when
        #          we are either not using the camera, or if we
        #          detect that the camera is pointing at the ground?
        self.led_relay = wpilib.Relay(led_relay_ch)
        self.led_relay.Set( Camera.LED_RELAY_ON )
        
    # TODO: NetworkTables interface from SmartDashboard

    
    def IsReadyToShoot(self):
        # TODO
        return True
    
    def SetCameraAngle(self, angle):
        '''Manually point the camera somewhere'''
        self.camera_angle = angle
    
        
    def Print(self):
        pass

        
    def Update(self):
    
        camera_angle = self.camera_angle
    
        if camera_angle is None:
            # TODO: get the angle that the SmartDashboard software wants us to
            # point the camera at
            camera_angle = Camera.DEFAULT_CAMERA_ANGLE
            
        self.servo.SetAngle( camera_angle )
        
        # reset
        self.camera_angle = None
