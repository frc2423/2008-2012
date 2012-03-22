
import msvcrt
import imp
import sys

import logging
import time

#sys.path.append( 'C:\\WindRiver\\workspace\\robotpy\\Packages\\wpilib' )

try:
    #SmartDashboard = imp.load_source( 'SmartDashboard', 'C:\\WindRiver\\workspace\\robotpy\\Packages\\wpilib\\package_wpilib\\SmartDashboard.py' )
    NetworkTables = imp.load_source( 'NetworkTables', 'C:\\WindRiver\\workspace\\robotpy\\Packages\\wpilib\\package_wpilib\\NetworkTables.py' )
    #import package_wpilib.SmartDashboard as SmartDashboard
    #import package_wpilib.NetworkTables as NetworkTables
except IOError as e:
    print( "Error loading the RobotPy installer library. (%s)\n\n" % e +
           "-- Is RobotPy source installed at 'C:\\WindRiver\\workspace\\robotpy'?" )
    msvcrt.getch()
    exit(1)

    
class SmartDashboardData:
    def GetType(self):
        raise NotImplementedError
    def GetTable(self):
        raise NotImplementedError

class SmartDashboardNamedData(SmartDashboardData):
    def GetName(self):
        raise NotImplementedError

class SmartDashboard:
    """The {@link SmartDashboard} class is the bridge between robot programs
    and the SmartDashboard on the laptop.

    When a value is put into the SmartDashboard here, it pops up on the
    SmartDashboard on the laptop.  Users can put values into and get values
    from the SmartDashboard."""
    @staticmethod
    def GetInstance():
        try:
            return SmartDashboard._instance
        except AttributeError:
            SmartDashboard._instance = SmartDashboard._inner()
            return SmartDashboard._instance

    class _inner:
        def __init__(self):
            self.table = NetworkTables.NetworkTable.GetTable("SmartDashboard")
            self.tablesToData = {}

        def __setitem__(self, keyName, value):
            if hasattr(value, "GetType") and hasattr(value, "GetTable"):
                # Assumed to be SmartDashboardData
                self.PutData(keyName, value)
            else:
                self.table[keyName] = value

        def __getitem__(self, keyName):
            v = self.table[keyName]
            if isinstance(v, NetworkTable):
                data = self.tablesToData.get(v)
                if data is None:
                    logging.error("SmartDashboardMissingKey: %s", keyName)
                return data
            else:
                return v

        def PutData(self, keyName=None, value=None):
            """Maps the specified key to the specified value in this table.
            The key can not be None.
            The value can be retrieved by calling the get method with a key
            that is equal to the original key.
            @param keyName the key
            @param value the value"""
            if keyName is None and hasattr(value, "GetName"):
                keyName = value.GetName()
            if keyName is None:
                logging.error("NullParameter: keyName")
                return
            if value is None:
                logging.error("NullParameter: value")
                return
            t = NetworkTables.NetworkTable()
            t.PutString("~TYPE~", value.GetType())
            t.PutSubTable("Data", value.GetTable())
            self.table[keyName] = t
            self.tablesToData[t] = value

        def GetData(self, keyName):
            """Returns the value at the specified key.
            @param keyName the key
            @return the value"""
            if keyName is None:
                logging.error("NullParameter: keyName")
                return None
            subtable = self.table.GetSubTable(keyName)
            data = self.tablesToData.get(subtable)
            if data is None:
                logging.error("SmartDashboardMissingKey: %s", keyName)
            return data

        def PutBoolean(self, keyName, value):
            """Maps the specified key to the specified value in this table.
            The key can not be NULL.
            The value can be retrieved by calling the get method with a key
            that is equal to the original key.
            @param keyName the key
            @param value the value"""
            self.table.PutBoolean(keyName, value)

        def GetBoolean(self, keyName):
            """Returns the value at the specified key.
            @param keyName the key
            @return the value"""
            return self.table.GetBoolean(keyName)

        def PutInt(self, keyName, value):
            """Maps the specified key to the specified value in this table.
            The keyName can not be None.
            The value can be retrieved by calling the get method with a key
            that is equal to the original key.
            @param keyName the key
            @param value the value"""
            self.table.PutInt(keyName, value)

        def GetInt(self, keyName):
            """Returns the value at the specified key.
            @param keyName the key
            @return the value"""
            return self.table.GetInt(keyName)

        def PutDouble(self, keyName, value):
            """Maps the specified key to the specified value in this table.
            The key can not be None.
            The value can be retrieved by calling the get method with a key
            that is equal to the original key.
            @param keyName the key
            @param value the value"""
            self.table.PutDouble(keyName, value)

        def GetDouble(self, keyName):
            """Returns the value at the specified key.
            @param keyName the key
            @return the value"""
            return self.table.GetDouble(keyName)

        def PutString(self, keyName, value):
            """Maps the specified key to the specified value in this table.
            Neither the key nor the value can be None.
            The value can be retrieved by calling the get method with a key
            that is equal to the original key.
            @param keyName the key
            @param value the value"""
            self.table.PutString(keyName, value)

        def GetString(self, keyName):
            """Returns the value at the specified key.
            @param keyName the key
            @return the value"""
            return self.table.GetString(keyName)
    

class Thing:

    def __init__(self):
        self.table = NetworkTables.NetworkTable()

        self.table['chamber'] = False
        self.table['middle_virtual'] = False
        self.table['feeder_top'] = False
        self.table['feeder_virtual'] = False
        self.table['feeder_low'] = True

    #
    # SmartDashboardData interface
    #
    def GetType(self):
        return "RobotWidget"

    def GetTable(self):
        return self.table
        

sd = SmartDashboard.GetInstance()
thing = Thing()
sd.PutData( "Thing1", thing )
        
import random
import time
        
while not msvcrt.kbhit():
    
    thing.table['chamber'] = bool(random.getrandbits(1))
    thing.table['middle_virtual'] = bool(random.getrandbits(1))
    thing.table['feeder_top'] = bool(random.getrandbits(1))
    thing.table['feeder_virtual'] = bool(random.getrandbits(1))
    thing.table['feeder_low'] = bool(random.getrandbits(1))
    
    time.sleep(2)

        
msvcrt.getch()
        
        