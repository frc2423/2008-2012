from ez_can_jaguar import EzCANJaguar

try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib

ANGLE_MOTOR_MIN_POSITION = 0
ANGLE_MOTOR_MAX_POSITION = 1.0
ANGLE_MOTOR_P = 100.0
ANGLE_MOTOR_I = 0.0
ANGLE_MOTOR_D = 0.0




class VerticalAngle(object):
    

    def __init__(self, angleCAN):
        self.goalVerticalAngle = 0
        self.currentVerticalAngle = 0 
        
        self.motor = EzCANJaguar(angleCAN, wpilib.CANJaguar.kPosition)
        self.motor.ConfigNeutralMode( wpilib.CANJaguar.kNeutralMode_Brake )
        self.motor.SetPositionReference(wpilib.CANJaguar.kPosRef_Potentiometer)
        self.motor.ConfigPotentiometerTurns( 1 )
        self.motor.SetPID( ANGLE_MOTOR_P, ANGLE_MOTOR_I, ANGLE_MOTOR_D )
        
        self.AutoMode = True
        
    
    #set angle to angle given in goalAngle
    def SetGoal(self,degrees):
        self.goalVerticalAngle = degrees
      
    
    #check if GoalAngle = CurrentAngle
    def IsReady(self):
        if self.goalVerticalAngle == self.motor.GetPosition():
            return True
            
    #Changes the mode 
    # Does this do anything
    def SetMode(self, AutoMode):
        self.AutoMode = AutoMode

    def Print(self):
        print("VerticalAngle:")
        print("    Current Angle: " + str(self.motor.GetPosition()) + "  Goal Angle: " + str(self.goalVerticalAngle()) + "  AutoMode: " + str(self.AutoMode))
        
    #update current and goal angles
    def Update(self):
        self.motor.Set(self.goalVerticalAngle)
        