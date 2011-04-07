'''
    Simple test harness for Kwarqs 2011 code. Along with fake_wpilib,
    this test harness takes advantage of the fact that nothing is
    private in python, and manipulates things to try and run through
    most of our code. 
    
    Really, this is only making sure we don't have any silly typos
    in the code before we send it to the robot. Will hopefully 
    prevent us from being taken down in a match because of a coding
    error that could have been prevented.. 
'''

import msvcrt

# state variables
disabled = True
autonomous_state_machine = None
operator_state_machine = None


def IsAutonomous():
    global autonomous_state_machine
    return autonomous_state_machine.run()

def IsDisabled():
    global disabled
    if disabled:
        disabled = False
        return True
    return False
    
def IsEnabled():
    return True

def IsOperatorControl():
    global operator_state_machine
    return operator_state_machine.run()

    
class AutonomousStateMachine(object):
    
    def __init__(self, robot):
        self.robot = robot
        self.loop = 1
        
    def run(self):
        ret = True
        print("[asm]: loop %d" % self.loop)
        
        if self.loop == 1:
            pass
        elif self.loop == 2:
            pass
            
        else:
            ret = False
        
        self.loop += 1
        return ret
    

class OperatorStateMachine(object):

    def __init__(self, robot):
        self.robot = robot
        self.loop = 1
        
    def run(self):
        ret = True
        print("[osm]: loop %d" % self.loop)
        
        if self.loop == 1:
            # simulate making the arm go up
            self.robot.arm_stick.buttons[0] = True
            self.robot.arm_stick.y = -1
            
        elif self.loop == 2:
            # press the hold button
            self.robot.arm_stick.buttons[1] = True
            
        elif self.loop == 3:
            # let go of it
            self.robot.arm_stick.buttons[1] = False
        
        elif self.loop == 4:
            # let go of the trigger
            self.robot.arm_stick.buttons[0] = False
            
        elif self.loop == 5:
            # thump motor activated
            self.robot.arm_stick.buttons[3] = True
            
        elif self.loop == 6:
            # thump motor in other direction
            self.robot.arm_stick.buttons[3] = False
            self.robot.arm_stick.buttons[4] = True
            
        elif self.loop == 7:
            # thump off
            self.robot.arm_stick.buttons[4] = False
            
        elif self.loop == 8:
            # tube out
            self.robot.arm_stick.buttons[5] = True
            
        elif self.loop == 9:
            # tube in
            self.robot.arm_stick.buttons[5] = False
            self.robot.arm_stick.buttons[6] = True
            
        elif self.loop == 10:
            # tube off
            self.robot.arm_stick.buttons[6] = False
            
        elif self.loop <= 20:
            # cause the first set of print statements to happen, in case 
            # we put something nasty in there
            pass
            
        else:
            ret = False
        
        self.loop += 1
        return ret
    
    
if __name__ == '__main__':

    # start it up
    from robot import run
    run() 
    
    # get the primary robot object
    from robot import robot
    
    
    # patch up the robot object so it works
    robot.IsAutonomous = IsAutonomous
    robot.IsDisabled = IsDisabled
    robot.IsEnabled = IsEnabled
    robot.IsOperatorControl = IsOperatorControl

    # go through disabled mode once
    disabled = True
    robot.Disabled()

    # go through autonomous mode at least once
    disabled = False
    autonomous_state_machine = AutonomousStateMachine(robot)
    robot.Autonomous()
    
    from fake_wpilib import global_time
    print("Fake time passed: %f" % global_time )
    
    # and do disabled again
    disabled = True
    robot.Disabled()

    # do at least one control loop execution
    disabled = False
    operator_state_machine = OperatorStateMachine(robot)
    robot.OperatorControl()
        
    from fake_wpilib import global_time
    print("Fake time passed: %f" % global_time )
    
    # and we're done... wait for the user to press a key, in case
    # they run it by double clicking it
    print()
    print("Press any key to continue")
    msvcrt.getch()
    
    