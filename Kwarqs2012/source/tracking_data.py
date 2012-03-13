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
        
        ''' doubles '''  
        TrackingData.distance = trackingDataTable.GetDouble(kDistance)
        TrackingData.sonar_distance = trackingDataTable.GetDouble(kSonarDistance)
        TrackingData.angle_susan = trackingDataTable.GetDouble(kAngleSusan)
        
        ''' ints '''
        TrackingData.x = trackingDataTable.GetInt(kX)
        TrackingData.y = trackingDataTable.GetInt(kY)
        TrackingData.valid_frames = trackingDataTable.GetInt(kValidFrames)
        
        ''' booleans '''
        TrackingData.target_data_valid = trackingDataTable.GetBoolean(kTargetDataValid)
        