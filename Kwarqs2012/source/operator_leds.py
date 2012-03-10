


try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib


    

    
class OperatorLEDs(object):
    
    '''
        We have 4 7-segment LEDs setup on the operator console. These are
        powered by 4 bcd-to-seven-segment decoders, with built-in latches.
        The LEDs are setup in two groups of two digits, so to save time we
        tie the latch enables together. 
        
        Because of the latency problems talking to the DS, we need to do 
        each digit in two-phase stages:
        
            1: enable x, set digits
            2: disable x
            3: enable y, set digits
            4: disable y
        
    '''
    
    # latch channels -- True latches the current output, False allows change
    G_EN = [ 3, 12 ]
 
    # the least significant digit channels
    # -> lsb to msb
    D0 = [ 9, 1, 5, 7 ]     # higher digit
    D1 = [ 15, 6, 11, 13 ]     # lower digit
    
    
    class DigitGroup(object):
        '''
            This controls the output of two digits. Do not
            create this class directly, get it from the 
            OperatorLEDs class instead.
        '''

        def __init__(self, eio, enable_ch ):
            
            self.value = None
            self.last_value = 0     # these are intentionally different
            
            self.eio = eio
            self.enable_ch = enable_ch
            
            # config the outputs
            self.eio.SetDigitalConfig( self.enable_ch, wpilib.DriverStationEnhancedIO.kOutput )
            
            # do the first digit display
            self._update(True)
            
        def _reset(self):
            self.last_value = -1
            
            
        def Set(self, value):
            '''value is either None or an integer between 0 and 99. If None,
               then the display is blanked'''
               
            if value is not None:
                if value < 0:
                    value = 0
                elif value > 99:
                    value = 99
                else:
                    value = int(value)
               
            self.value = value
            
        #
        # Internal functions
        #
            
        def _update_digit(self, value, chs):
            
            #print( "Update: %s %s" % (value, chs) )
            
            if value is None:
                value = 0xf     # this blanks the input
                
            for i in range(0,4):
                if int(value) & (1 << i):
                    self.eio.SetDigitalOutput( chs[i], True )
                else:
                    self.eio.SetDigitalOutput( chs[i], False )
                    
        def _wants_update(self):
            return self.value == self.last_value
            
        def _update(self, enable):
            '''Returns True if it set digits'''
 
            if not enable:
                self.eio.SetDigitalOutput( self.enable_ch, True )
                return False
                
            self.eio.SetDigitalOutput( self.enable_ch, False )
            
            if self.value is None:
                digit0 = None
                digit1 = None
            else:
                digit0 = self.value % 10    # lower digit
                digit1 = self.value / 10    # higher digit
            
            self._update_digit( digit0, OperatorLEDs.D0 )
            self._update_digit( digit1, OperatorLEDs.D1 )
            
            self.last_value = self.value
                
            return True
 
 
    def __init__(self):
    
        # initialize the io
        eio = wpilib.DriverStation.GetInstance().GetEnhancedIO()
        self.current = 0
        self.current_enabled = False
        self.stage = 0
    
        for ch in OperatorLEDs.D0:
            eio.SetDigitalConfig( ch, wpilib.DriverStationEnhancedIO.kOutput )
            
        for ch in OperatorLEDs.D1:
            eio.SetDigitalConfig( ch, wpilib.DriverStationEnhancedIO.kOutput )
    
        self.groups = []
        self.groups.append( OperatorLEDs.DigitGroup(eio, OperatorLEDs.G_EN[0]) )
        self.groups.append( OperatorLEDs.DigitGroup(eio, OperatorLEDs.G_EN[1]) )
        
        
    def GetDigitGroup(self, group):
        '''group is a value between 1 and 2'''
        return self.groups[group-1]
    
    def Reset(self):
        '''Call this when you enter OperatorControl mode'''
        for i in self.groups:
            i._reset()
    
    def Update(self):
        '''
            This function should only be called when there is new data
            received from the driver station. We intentionally do not
            check for that here, in case other parts of the program need
            to depend on that data.
        '''
        
        # if self.current == 0:
            # first = 0
            # second = 1
        # else:
            # first = 1
            # second = 0
        
        # can only switch groups if it is not enabled
        # if not self.current_enabled:

            # self.groups[second]._update(True)
            # self.current = second
            # self.current_enabled = True
            
        # else:
        
            # if it doesn't want to switch, then don't bother
            # if not self.groups[second]._wants_update():
                # self.groups[first]._update(True)
            # else:
                # self.groups[first]._update(False)
                # self.current_enabled = False
                
        
        if self.stage == 0:
            self.groups[0]._update(True)
            self.groups[1]._update(False)
            
        elif self.stage == 1:
            self.groups[0]._update(False)
            self.groups[1]._update(False)
        
        elif self.stage == 2:
            self.groups[0]._update(False)
            self.groups[1]._update(True)
        
        elif self.stage == 3:
            self.groups[0]._update(False)
            self.groups[1]._update(False)
        
        if self.stage == 3:
            self.stage = 0
        else:
            self.stage += 1