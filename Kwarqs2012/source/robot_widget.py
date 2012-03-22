

try:
    import wpilib.SmartDashboard
    from wpilib.NetworkTables import NetworkTable
except ImportError:
    from fake_wpilib.NetworkTables import NetworkTable


class RobotWidget(object):
    '''
        This is an interface to a custom widget on the SmartDashboard. It's 
        pretty sweet. Each of the elements in the NetworkTable is a bool, and
        when the bool is True, an image is overlaid on an image of the robot. 
    '''

    def __init__(self, name):
    
        # add this to the SmartDashboard automatically
        sd = wpilib.SmartDashboard.SmartDashboard.GetInstance()
        sd.PutData( name, self )
    
        self.table = NetworkTable()

        self.table['chamber'] = False
        self.table['middle_virtual'] = False
        self.table['feeder_top'] = False
        self.table['feeder_virtual'] = False
        self.table['feeder_low'] = False
        
    def SetAll(self, chamber, middle_virtual, feeder_top, feeder_virtual, feeder_low):
        self.table['chamber'] = chamber
        self.table['middle_virtual'] = middle_virtual
        self.table['feeder_top'] = feeder_top
        self.table['feeder_virtual'] = feeder_virtual
        self.table['feeder_low'] = feeder_low
        
    def SetChamber(self, value):
        self.table['chamber'] = value
        
    def SetMiddleVirtual(self, value):
        self.table['middle_virtual'] = value
        
    def SetFeederTop(self, value):
        self.table['feeder_top'] = value
        
    def SetFeederVirtual(self, value):
        self.table['feeder_virtual'] = value
        
    def SetFeederLow(self, value):
        self.table['feeder_low'] = value
    

    #
    # SmartDashboardData interface
    #
    def GetType(self):
        return "RobotWidget"

    def GetTable(self):
        return self.table

