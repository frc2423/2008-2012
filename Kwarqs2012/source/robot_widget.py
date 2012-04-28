

try:
    import wpilib.SmartDashboard
    from wpilib.NetworkTables import NetworkTable
except ImportError:
    import fake_wpilib as wpilib
    import fake_wpilib.SmartDashboard
    from fake_wpilib.NetworkTables import NetworkTable


class RobotWidget(object):
    '''
        This is an interface to a custom widget on the SmartDashboard. It's 
        pretty sweet. Each of the elements in the NetworkTable is a bool, and
        when the bool is True, an image is overlaid on an image of the robot. 
    '''

    def __init__(self, name):
    
        # add this to the SmartDashboard automatically
        self.table = NetworkTable()
        
        self.table['chamber'] = False
        self.table['feeder_top'] = False
        self.table['feeder_mid'] = False
        self.table['feeder_low'] = False
        self.table['feeder_entrance'] = False
        
        sd = wpilib.SmartDashboard.SmartDashboard.GetInstance()
        sd.PutData( name, self )

        
        
    def SetAll(self, chamber, feeder_top, feeder_mid, feeder_low, feeder_entrance):
        self.table['chamber'] = bool(chamber)
        self.table['feeder_top'] = bool(feeder_top)
        self.table['feeder_mid'] = bool(feeder_mid)
        self.table['feeder_low'] = bool(feeder_low)
        self.table['feeder_entrance'] = bool(feeder_entrance)
        
        
    def SetChamber(self, value):
        self.table['chamber'] = value
        
        
    def SetFeederTop(self, value):
        self.table['feeder_top'] = value
        
    def SetFeederMid(self, value):
        self.table['feeder_mid'] = value
        
    def SetFeederLow(self, value):
        self.table['feeder_low'] = value
        
    def SetFeederEntrance(self, value):
        self.table['feeder_entrance'] = value
    

    #
    # SmartDashboardData interface
    #
    def GetType(self):
        return "RobotWidget"

    def GetTable(self):
        return self.table

