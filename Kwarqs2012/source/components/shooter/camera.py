
from components.shooter.susan import Susan

try:
    import wpilib
    import wpilib.SmartDashboard
    from   wpilib.NetworkTables import NetworkTable

except ImportError:
    import fake_wpilib as wpilib
    import fake_wpilib.SmartDashboard
    from fake_wpilib.NetworkTables import NetworkTable
    
''' constants used to get data from the table '''
TABLENAME = "TrackingData"
DISTANCE = "distance"
SONAR_DISTANCE = "sonar_distance"
ANGLE_SUSAN = "angle_susan"
X = "x"
Y = "Y"
TARGET_DATA_VALID = "target_data_valid"
VALID_FRAMES = "valid_frames"

trackingDataTable = NetworkTable.GetTable(TABLENAME)    

''' Angle tolerance is the same with the susan and camera '''
ANGLE_TOLERANCE = Susan.ANGLE_TOLERANCE
    
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
    DEFAULT_CAMERA_ANGLE = 70.0
    
    LED_RELAY_ON    = wpilib.Relay.kForward 
    LED_RELAY_OFF   = wpilib.Relay.kOff
    
    
    def __init__(self, camera_servo_ch, led_relay_ch):
        
        self.servo = wpilib.Servo(camera_servo_ch)
        self.camera_angle = None
        self.current = None
        
        # this never changes, set it and forget it
        # -> TODO: Should we turn this off to conserve power when
        #          we are either not using the camera, or if we
        #          detect that the camera is pointing at the ground?
        self.led_relay = wpilib.Relay(led_relay_ch)
        self.led_relay.Set( Camera.LED_RELAY_ON )
        
        self.trackingData = TrackingData();
        
    
    def IsReadyToShoot(self):
        # TODO
        ''' angle is with in the tolerance level so we are ready to shoot'''
        if self.trackingData.angle_susan < ANGLE_TOLERANCE or self.trackingData.angle_susan > -ANGLE_TOLERANCE:
            return True
        else:
            return False
    
    def SetAngle(self, angle):
        '''Manually point the camera somewhere'''
        self.camera_angle = angle
    
        
    def Print(self):
        print("Camera: %s" % self.current)

        
    def Update(self):
    
        camera_angle = self.camera_angle
        
        self.trackingData.Update()
    
        if camera_angle is None:
            # TODO: get the angle that the SmartDashboard software wants us to
            # point the camera at
            camera_angle = Camera.DEFAULT_CAMERA_ANGLE
            
        self.servo.SetAngle( camera_angle )
        
        # reset
        self.current = camera_angle
        self.camera_angle = None
        
class TrackingData(object):
    
    '''
        This class refers to the tracking data that is processed on the driverstation
        from the camera image. An instance is to be created inside the camera class to
        update its variables
    ''' 
    
    ''' doubles '''   
    distance = 0.0
    sonar_distance = 0.0
    angle_susan = 0.0
    
    ''' ints '''  
    x = 0
    y = 0
    valid_frames = 0
    
    ''' booleans '''
    target_data_valid = False
        
    def Update(self):
        ''' Updates Values from the Network Table '''
        
        ''' begins transaction '''
        trackingDataTable.BeginTransaction()
        
        ''' doubles '''  
        TrackingData.distance = trackingDataTable.GetDouble(DISTANCE)
        TrackingData.sonar_distance = trackingDataTable.GetDouble(SONAR_DISTANCE)
        TrackingData.angle_susan = trackingDataTable.GetDouble(ANGLE_SUSAN)
        
        ''' ints '''
        TrackingData.x = trackingDataTable.GetInt(X)
        TrackingData.y = trackingDataTable.GetInt(Y)
        TrackingData.valid_frames = trackingDataTable.GetInt(VALID_FRAMES)
        
        ''' booleans '''
        TrackingData.target_data_valid = trackingDataTable.GetBoolean(TARGET_DATA_VALID)
        
        '''Ends Transaction'''
        trackingDataTable.EndTransaction()
