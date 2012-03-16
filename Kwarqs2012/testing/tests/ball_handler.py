'''
Things to test:

    - ball transfer logic
    - 'phantom' ball logic
    - ball counting
    - edge cases for empty balls

 
'''

from fake_wpilib import GetClock


# directory of robot.py, relative to this file
robot_path = '../../source'


class BallSimulator(object):

    def __init__(self, feeder, chamber):
    
        self.feeder = feeder
        self.chamber = chamber
        self.Reset()
        
    
    def Reset(self):
        # not in, low, virt, hi, virt, chamber, out
        self.balls = [None]*7
        
        # number of iterations needed to move to the next position
        self.steps = [1, 5, 5, 5, 7, 10, 10]
        
        self.exited_bottom = 0
        self.exited_top = 0
        self._moved = False
        
    
    def AddBall(self):
        if self.balls[0] != None:
            raise RuntimeError("Error: ball already at bottom")
            
        self.balls[0] = 0
        
    def Moved(self):
        return self._moved
    
    def Print(self):
        print( "  Balls: %s" % ', '.join( '%s: %s' % (i, ball) for i,ball in enumerate(self.balls)))

    def _set_sensors(self):
        '''Depending on where balls are, set them'''
        
        if self.balls[1] is not None:
            self.feeder.low_ball_sensor.value = True
        else:
            self.feeder.low_ball_sensor.value = False
            
        if self.balls[3] is not None:
            self.feeder.top_ball_sensor.irSensor.voltage = 5
        else:
            self.feeder.top_ball_sensor.irSensor.voltage = 0
            
        if self.balls[5] is not None:
            self.chamber.ball_sensor.irSensor.voltage = 5
        else:
            self.chamber.ball_sensor.irSensor.voltage = 0
    
    def _up(self, i):
        if self.balls[i] is not None:
            self.balls[i] += 1
            self._moved = True
            
            # next stage?
            if self.balls[i] > self.steps[i]:
                if i+1 == len(self.balls):
                    print("[%.2f] Ball left top" % GetClock())
                    self.balls[i] = None
                    self.exited_top += 1
                elif self.balls[i+1] is None:
                    self.balls[i+1] = 0
                    self.balls[i] = None
               
    def _down(self, i):
        if self.balls[i] is not None:
            self.balls[i] -= 1
            self._moved = True
            
            # next stage?
            if self.balls[i] < 0:
                if i == 0:
                    print( "[%.2f] Ball left bottom" % GetClock() )
                    self.balls[i] = None
                    self.exited_bottom += 1
                elif self.balls[i-1] is None:
                    self.balls[i] = None
                    self.balls[i-1] = self.steps[i]
    
    
    def Simulate(self):
        '''Must be called every loop iteration'''
        
        self._moved = False
        
        if self.chamber.belt_motor.value == self.chamber.BELT_UP:
            self._up(6)
            self._up(5)
            
            if self.feeder.belt_motor.value == self.feeder.BELT_UP:
                self._up(4)
            
        if self.feeder.belt_motor.value == self.feeder.BELT_UP:
            
            self._up(3)
            self._up(2)
            self._up(1)
            self._up(0)
            
        elif self.feeder.belt_motor.value == self.feeder.BELT_DOWN:
        
            self._down(0)
            self._down(1)
            self._down(2)
            self._down(3)
        
            if self.chamber.belt_motor.value == self.chamber.BELT_DOWN:
                self._down(4)
            
        if self.chamber.belt_motor.value == self.chamber.BELT_DOWN:
            self._down(5)
            self._down(6)
            
        # set the sensors accordingly
        self._set_sensors()


class Test(object):

    def __init__(self, robot_module, myrobot):
        self.robot_module = robot_module
        self.myrobot = myrobot
        
        self.switches = []
        self.switches.append( self.robot_module.AUTO_BALL_HANDLER_SWITCH )
        self.switches.append( self.robot_module.CONTINUOUS_BALL_HANDLER_SWITCH )
        
        self.ds = self.robot_module.wpilib.DriverStation.GetInstance()
        self.eio = self.ds.GetEnhancedIO()
        
        self.chamber = self.robot_module.chamber
        self.feeder = self.robot_module.feeder
        self.ball_handler = self.robot_module.ballHandler
        
        self.balls = BallSimulator(self.feeder, self.chamber)
        
        
    def Reset(self):
        self.loop_count = 0
        self.balls.Reset()
        
    def _get_dir(self, object):
        '''Gets the direction of a feeder or chamber as a string'''
        if object.belt_motor.value == object.BELT_UP:
            return "up"
        elif object.belt_motor.value == object.BELT_DOWN:
            return "down"
        elif object.belt_motor.value == object.BELT_OFF:
            return "off"
            
        raise RuntimeError("Invalid state")
    
    def Print(self):
        '''Prints the current status of things'''
        print("Loop %s:" % self.loop_count )
        print("  Chamber: %s" % self._get_dir( self.chamber) )
        print("  Feeder : %s" % self._get_dir( self.chamber) )
        print("  Handler: fvirt: %s mvirt: %s" % (self.ball_handler.feeder_virtual_ball, self.ball_handler.middle_virtual_ball))
        self.balls.Print()
        print("")
        
    
        
    def SingleContinuous(self, tm):
        '''Tests the easy case -- a single ball coming up through the feeder/chamber'''
    
        done = False

        for switch in self.switches:
            self.eio.digital[switch] = False
        
        if self.loop_count == 0:
            self.balls.AddBall()
        
        self.balls.Simulate()
        self.balls.Print()
        
        if self.loop_count > 0 and not self.balls.Moved():
            if self.chamber.IsFull():
                # once it gets to the chamber and stops, continue it onwards
                stick,button = self.robot_module.SHOOT_BALL_BUTTON
                self.ds.stick_buttons[ stick ][ button ] = True
            else:
                print("ERROR: No balls moved this cycle")
                done = True
        
        if self.balls.exited_top > 1:
            print("SUCCESS: Ball exited the top")
            done = True
            
        # infinite loop protection
        if tm > 105.0:
            print("FAILED: Time ran out")
            done = True
            
        self.loop_count += 1
        return not done
        

def run_tests( robot_module, myrobot ):

    test = Test( robot_module, myrobot )

    myrobot.enabled = True
    
    test.Reset()
    myrobot.on_IsOperatorControl = test.SingleContinuous
    
    myrobot.OperatorControl()
    
    # next test.. 


