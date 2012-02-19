try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib

ANGLE_MOTOR_MIN_POSITION = 0.0
ANGLE_MOTOR_MAX_POSITION = 1.0
ANGLE_MOTOR_P = 100.0
ANGLE_MOTOR_I = 0.0
ANGLE_MOTOR_D = 0.0


def _translate_z_to_angle_motor_position( z ):

    # P = Xmax - (Ymax - Y)( (Xmax - Xmin) / (Ymax - Ymin) )
    value = ANGLE_MOTOR_MAX_POSITION - ((1 - z)*( (ANGLE_MOTOR_MAX_POSITION - ANGLE_MOTOR_MIN_POSITION) / (1.0-0) ) )
    
    if value > ANGLE_MOTOR_MAX_POSITION:
        return ANGLE_MOTOR_MAX_POSITION
    elif value < ANGLE_MOTOR_MIN_POSITION:
        return ANGLE_MOTOR_MIN_POSITION
    return value

class VerticalAngle(object):
    

    def __init__(self, angleCAN, anglePot):
        self.verticalGoalAngle = 0
        self.verticalCurrentAngle = 0 
        
        self.motor = wpilib.CANJaguar(angleCAN)
        self.motor.ConfigNeutralMode( wpilib.CANJaguar.kNeutralMode_Brake )
        self.motor.SetPositionReference(wpilib.CANJaguar.kPosRef_Potentiometer)
        self.motor.ConfigPotentiometerTurns( 1 )
    #set angle to angle given in goalAngle
    def SetGoal(self,degrees):
        self.goalVeticalAngle = degrees
      
    
    #check if GoalAngle = CurrentAngle
    def IsReady(self):
        if self.verticalGoalAngle == self.verticalCurrentAngle:
            return True
            
    
    #update current and goal angles
    def Update(self):
        self.motor.Set(self.verticalCurrentAngle)
        self.currentAngle = self.currentAngle.GetPosition()