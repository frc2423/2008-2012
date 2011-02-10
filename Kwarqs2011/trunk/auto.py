import wpilib

class Auto(object):

    def __init__(self):
        '''Constructor'''
        pass
        
    
        
    def do_control_loop(self, drive, arm):
        '''Implements automated tube placement'''
        
        
        
        # if arm is at height, check if we are in deploy mode

        if arm.arm_is_in_position():
            if robot.deploy_range()  
                speed = 0
                if sensor.middle_sensor_on():
                    arm.deploy_tube()
                else: 
                    self.line_tracking()
            else:   
                speed = 1
                self.line_tracking()
                
        else:
        
        
        
        
        
            if robot_too_close():
                stop_motors
        # else if, use line tracking
        else:
            robot.auto.do_control_loop()
                
            
        # else if, set specs and use line tracking 
        else:
            specs == ON
            robot.auto.do_control_loop()
        
        # if the middle sensor is on, deploy tube
        if sensor.middle_sensor_on():
            arm.deploy_tube()
            
        # else, go to line tracking
        else:
            robot.auto.do_control_loop()
        
    # note: deploy_range is made up
        # else, check if we are too close
        # if we are too close, stop motors
        # if we are in deploy mode, turn the specs off and check if the middle sensor is on

