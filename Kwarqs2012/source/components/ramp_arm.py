#arm = Jaguar(3)

#needs to be both directions
#go down as long button is pressed
#if button not pressed off
#if different button pressed reverse

class RampArm(object):

    forwardSpeed = 1
    reverseSpeed = -0.5


    #
    #   Descrition: Initilize a RampArm Instance
    #   
    #   Variables:
    #   int mNumber: PWM connection number for motor
    def __init__(self, mNumber):
        self.arm = Jaguar(mNumber)
        self.armSpeed = 0
    
    def LowerRamp(self):
        self.armSpeed = forwardSpeed #sets full power forward
  
#if lowerRamp is called the self.armSpeed is fowardSpeed and the motor is set to full power
#then sets self.armspeed is -.5
#then when LowerRamp is not needed then the motor is set to -.5
  
    def Update(self):
        if self.armSpeed == forwardSpeed:
            self.arm.Set(forwardSpeed)
            self.armSpeed = reverseSpeed
        else
            self.arm.Set(reverseSpeed)
            
            