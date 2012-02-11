import wpilib

stick = wpilib.joystick(2)
motor = wpilib.jaguar(3)
encoder = wpilib.encoder(10,11)

class buttontest(wpilib.simplerobot):

    def __init__(self):
        wpilib.SimpleRobot.__init__(self)
        encoder.Start()
    
    def rotateone():
        motor.Set( stick.GetButton(1) )

def run():            
    robot = buttontest()
    robot.StartCompetition