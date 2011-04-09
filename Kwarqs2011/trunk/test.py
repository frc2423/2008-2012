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

from optparse import OptionParser


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
        self.wall_distance = 120
        
    def run(self):
        ret = True
        #print("[asm]: loop %d" % self.loop)
        
        # increase/decrease the wall distance depending on our speed
        self.wall_distance += self.robot.drive.y*2
        
        # set it
        self.robot.auto.ultrasonic.distance = self.wall_distance
        
        # at 1.5 seconds in, pretend we hit the bottom for our calibration,
        # and then inch the vertical motor closer to our destination
        if self.loop == 30:
            self.robot.arm.vertical_motor.reverse_ok = False
        
        if self.loop > 30:
            self.robot.arm.vertical_motor.position += (self.robot.arm.vertical_motor.value - self.robot.arm.vertical_motor.position )/16.0
        
        #if self.loop == 160:
        #    self.robot.arm.print_diagnostics()
        
        # autonomous is done after 15 seconds
        if self.loop > 20*15:
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
        
        self.robot.drive_stick.buttons[9] = True
        
        if self.loop == 1:
            # simulate making the arm go up
            self.robot.arm_stick.buttons[0] = True
            self.robot.arm_stick.y = -1
            self.robot.drive_stick.buttons[6] = True
            
        elif self.loop == 2:
            # press the hold button
            self.robot.arm_stick.buttons[1] = True
            self.robot.drive_stick.buttons[6] = False
            
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
            
        elif self.loop == 21:
            # set the thump motor position
            self.robot.arm_stick.buttons[9] = True
            self.robot.arm_stick.z = 1.0
            
        elif self.loop == 22:
            # disable thump motor positioning
            self.robot.arm_stick.buttons[9] = False
            
        else:
            ret = False
        
        self.loop += 1
        return ret
    
    
if __name__ == '__main__':

    parser = OptionParser()
    
    parser.add_option(  '--no-user-control',
                        action='store_true', dest='no_user_control', default=False,
                        help='Do not run the operator control mode tests' )
                        
    parser.add_option(  '--fms',
                        action='store_true', dest='fms_attached', default=False,
                        help='Pretend that the robot is on the field' )
    
    (options, args) = parser.parse_args()


    # start it up
    from robot import run
    run() 
    
    # get the primary robot object
    from robot import robot
    
    robot.ds.fms_attached = options.fms_attached
    
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
    
    # do operator control
    if not options.no_user_control:
    
        # and do disabled again
        disabled = True
        robot.Disabled()
    
        disabled = False
        operator_state_machine = OperatorStateMachine(robot)
        robot.OperatorControl()
        
        from fake_wpilib import global_time
        print("Fake time passed: %f" % global_time )
    
    