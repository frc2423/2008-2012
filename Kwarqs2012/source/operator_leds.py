


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
    '''
    
    # latch channels -- True latches the current output, False allows change
    G_EN = [ 0, 1 ]
 
    # the least significant digit channels
    # -> lsb to msb
    D0 = [ 2, 3, 4, 5 ]     # lower digit
    D1 = [ 6, 7, 8, 9 ]     # higher digit
    
    
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
            
            if value is None:
                value = 0xf     # this blanks the input
                
            for i in range(0,3):
                if value & (1 << i):
                    self.eio.SetDigitalOut( chs[i], True )
                else:
                    self.eio.SetDigitalOut( chs[i], False )
            
        def _update(self, enable):
            '''Returns True if it set digits'''
 
            if not enable:
                self.eio.SetDigitalOut( self.enable_ch, True )
                return False
                
            # don't bother changing the display if it hasn't changed
            if self.value == self.last_value:
                return False
                
            
            self.eio.SetDigitalOut( self.enable_ch, False )
            
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
    
        for ch in OperatorLEDs.D0:
            eio.SetDigitalConfig( ch, wpilib.DriverStationEnhancedIO.kOutput )
            
        for ch in OperatorLEDs.D1:
            eio.SetDigitalConfig( ch, wpilib.DriverStationEnhancedIO.kOutput )
    
        self.groups = []
        self.groups.append( DigitGroup(eio, OperatorLEDs.G_EN[0]) )
        self.groups.append( DigitGroup(eio, OperatorLEDs.G_EN[1]) )
        
        
    def GetDigitGroup(self, group):
        '''group is a value between 1 and 2'''
        return self.groups[group-1]
    
    
    def Update(self):
        '''
            This function should only be called when there is new data
            received from the driver station. We intentionally do not
            check for that here, in case other parts of the program need
            to depend on that data.
        '''
        
        if self.last_group == 0:
            first = 1
            second = 0
        else:
            first = 0
            second = 1
        
        # if the first group declines to update, then
        # try to update the second group. But if it
        # does the update, then the enable line definitely
        # should be switched
        
        # TODO: There has to be race conditions here... oh
        # well, let's see how well it works, not much we can
        # do without rewriting WPILib
        
        if self.groups[first]._update(True):
            self.groups[second]._update(False)
            self.last_group = first
        else:    
            self.groups[second]._update(True)
        
        