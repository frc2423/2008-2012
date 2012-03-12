
try:
    import wpilib
except:
    import fake_wpilib as wpilib

class PrintTimer(object):
    '''This is a useful class that we can use to determine if we
    want to print something or not. Better than the WPILib timer
    class, because it doesn't play catchup... '''

    def __init__(self, timeout=1.0):
        self.timer = wpilib.Timer()
        self.timer.Start()
        self.timeout = timeout
        # we subtract the timeout because we want should_print()
        # to return true the very first time that this is called
        self.last_time = self.timer.Get() - (timeout*2)
        self.ids = []
        
    def Get(self):
        return self.timer.Get()
        
    def should_print(self, id=None):
        '''
        This returns true if the timeout interval has expired. 
        If you specify an ID, then this will return true for
        every ID that hasn't printed in the last interval
        '''
        
        if self.last_time + self.timeout < self.timer.Get():
            self.last_time = self.timer.Get()
            self.ids = [id]
            return True
        elif id not in self.ids:
            self.ids.append( id )
            return True
            
        return False
        

class ConsoleLED(object):
    '''
        This implements primitives to control an LED on the driver
        station. 
        
        At the end of every control loop, you must call Update() to actually
        set the LED on the driver station
    '''
        
    def __init__(self, port, enhanced_io=True):
        '''
            port is the port on the driver station to set
            if enhanced_io is True then it will used enhanced I/O
        '''
        
        self.port = port
        self.value = False
        
        # none if no blink, otherwise its the blink period
        self.blink = None       
        self.blink_timer = wpilib.Timer()
        
        ds = wpilib.DriverStation.GetInstance()
        
        if enhanced_io:
            eio = ds.GetEnhancedIO()
            eio.SetDigitalConfig( self.port, wpilib.DriverStationEnhancedIO.kOutput )
            self.set_fn = eio.SetDigitalOutput
        else:
            self.set_fn = ds.SetDigitalOut
        
        
    def Blink(self, period=0.25):
        '''Blinks an LED with the specified period in seconds'''
        if self.blink is not None:
            self.blink_timer.Reset()
            
        self.blink = period / 2.0
        self.value = not self.value
        
    def TurnOn(self):
        self.Set(True)
        
    def TurnOff(self):
        self.Set(False)
        
    def Set(self, value):
        self.blink = None
        self.value = True
        
    def Update(self):
    
        if self.blink is not None:
            if self.blink_timer.HasPeriodPassed( self.blink ):
                self.value = not self.value
        
        self.set_fn( self.port, self.value )
        