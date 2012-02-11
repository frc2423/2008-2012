
import fake_wpilib as wpilib
import robot

class OperatorControlLoop(object):

    loop_count = 0

    def IsOperatorControl(self):

        if self.loop_count == 2:
            robot.stick1.buttons[0] = True
        else:
            robot.stick1.buttons[0] = False
    
        if self.loop_count == 52:
            robot.stick1.buttons[1] = True
        else:
            robot.stick1.buttons[1] = False

        robot.encoder.value += int( robot.motor.value * 100 )
    
        print( "Motor: %f; Encoder: %d" % (robot.motor.value, robot.encoder.value) )
    
        self.loop_count += 1
        
        if self.loop_count > 100:
            return False
    
        return True


if __name__ == '__main__':

    print( "Initializing robot... " )
    myrobot = wpilib.initialize_robot()
    
    loop = OperatorControlLoop()
    
    myrobot.enabled = True
    myrobot.on_IsOperatorControl = loop.IsOperatorControl
    
    myrobot.OperatorControl()
    
    
    print("Test completed.")
    
    
    
    

