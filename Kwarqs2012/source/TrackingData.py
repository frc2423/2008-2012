'''
    Implementation of Tracking Data obtained from camera and processed on the 
    drivers station. Every time update is called we get the information from the
    network about the new values from the driverstation
'''

try:
    import wpilib
    from wpilib.NetworkTables import NetworkTable
    
except ImportError:

    import fake_wpilib as wpilib

''' constants used to get data from the table '''
kTableName = "TrackingData"
kDistance = "distance"
kSonarDistance = "sonar_distance"
kAngleSusan = "angle_susan"
kX = "x"
kY = "Y"
kTargetDataValid = "target_data_valid"
kFrameNumber = "frame_number"
kValidFrames = "valid_frames"


trackingDataTable = NetworkTable.GetTable(kTableName)

class TrackingData(object):
    
    def __init__(self):
        ''' Constructor '''
        ''' Initialize all variables used by robot from the vision system '''
        
        ''' doubles '''   
        self.distance = 0.0
        self.sonar_distance = 0.0
        self.angle_susan = 0.0
        
        ''' ints '''  
        self.x = 0
        self.y = 0
        self.valid_frames = 0
        
        ''' booleans '''
        self.target_data_valid = False
        
    def Update(self):
        ''' Updates Values from the Network Table '''
        
        ''' doubles '''  
        self.distance = trackingDataTable.GetDouble(kDistance)
        self.sonar_distance = trackingDataTable.GetDouble(kSonarDistance)
        self.angle_susan = trackingDataTable.GetDouble(kAngleSusan)
        
        ''' ints '''
        self.x = trackingDataTable.GetInt(kX)
        self.y = trackingDataTable.GetInt(kY)
        self.valid_frames = trackingDataTable.GetInt(kValidFrames)
        
        ''' booleans '''
        self.target_data_valid = trackingDataTable.GetBoolean(kTargetDataValid)
        