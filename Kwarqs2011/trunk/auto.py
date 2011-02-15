import wpilib

class Auto(object):

    def __init__(self):
        '''Constructor'''
        pass
        
    def do_control_loop(self, drive, arm):
        '''Implements automated tube placement'''
        
        # Is arm at height?
            # Yes! -> In deploy range?
                # Yes! -> Turn speed off, then...
                # Is the middle sensor on?
                    # Yes! -> Deploy tube
                    # No! -> Line tracking
                # No! -> Set speed, then...
                    # Line tracking
            # No! -> Are we too close?
                # Yes! -> Stop motors
                # No! -> Line tracking