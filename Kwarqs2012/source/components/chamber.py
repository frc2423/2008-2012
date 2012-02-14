''' 
Chamber needs to shoot a ball, know when its full, know when empty.
'''

try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib

class Chamber ( Object ):

    def __Init__ ( self, chamberCANJagNum, limitSwitchNum ):
        chambermotor = CANJaguar(chamberCANJagNum)
        limitswitch3 = LimitSwitch(limitSwitchNum)
        
    def Release( self ):
        chambermotor.Set(1)
        
    def IsFull( self ):
        return( limitswitch3 ==  True )
        
    def Empty( self ):
        return( limitswitch3 == False )
        
    def IsReady( self ):  
        return( IsFull == True )    
        
    def Update( self ):
        
        
    