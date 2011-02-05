import wpilib

KICKER_OUT = 1
WAIT = 2
KICKER_IN = 3
WAIT2 = 4

class Kicker(object):

    def __init__(self):
        self.compressor = wpilib.Compressor(6,1)
        self.compressor.Start()
        self.solenoid1 = wpilib.Solenoid(1) 
        self.solenoid2 = wpilib.Solenoid(2)
        self.timer = wpilib.Timer()
        self.kicker_in()
        self.state = KICKER_OUT
        self.timer.Start()  
        
    def kicker_fn(self,stick):
        #1.Kicker out
        #2.Wait 1 second
        #3.Kicker in
        #4.wait 1 second
        
        if self.state is KICKER_OUT:
            if stick.GetTrigger():
                self.kicker_out()
                self.timer.Reset()
                self.state = WAIT
                
        elif self.state is WAIT:
            if self.timer.Get() > 1.0:
                self.state = KICKER_IN
                
        elif self.state is KICKER_IN:
            self.kicker_in()
            self.timer.Reset()
            self.state = WAIT2
            
        elif self.state is WAIT2:
            if self.timer.Get() > 1.0:
                self.state = KICKER_OUT
                
        else:
            raise Exception("Oh noes! Invalid state!")
        
    def kicker_out(self): 
        self.solenoid1.Set(True) 
        self.solenoid2.Set(False) 
    
    
    def kicker_in(self): 
        self.solenoid1.Set(False)
        self.solenoid2.Set(True)

        
#########################################
# testing functions to verify correctness
#########################################
        
def test_kicker_out(kicker):
    
    # run the function that we're testing
    kicker.kicker_out()

    # check to see if the things we expected to happen happened
    if kicker.solenoid1.is_set == True and kicker.solenoid2.is_set == False:
        print("test_kicker_out(): WIN")
    else:
        print("test_kicker_out(): FAIL")
        

def test_kicker_in(kicker):
    
    # run the function that we're testing
    kicker.kicker_in()
    
    # check to see if the things we expected to happen happened
    if kicker.solenoid1.is_set == False and kicker.solenoid2.is_set == True:
        print("test_kicker_in(): WIN")
    else:
        print("test_kicker_in(): FAIL")
    
def test_kicker_fn(kicker):
    stick = wpilib.Joystick()

    # 1. Determine inputs needed to stimulate tested object
    # 2. Determine required outputs to test to determine required results
    # 3. Find an effective testing methodology

    # make sure this happens! :D
    assert(kicker.state is KICKER_OUT)
    
    kicker.kicker_fn(stick)
    assert(kicker.state is WAIT) 
    
    kicker.timer.Set(1.1) 
    kicker.kicker_fn(stick)
    assert(kicker.state is KICKER_IN)
    
    kicker.kicker_fn(stick)
    assert(kicker.state is WAIT2)
    
    kicker.timer.Set(1.1)
    kicker.kicker_fn(stick)
    assert(kicker.state is KICKER_OUT)
     
if __name__ == '__main__':
    
    # create kicker object
    kicker = Kicker()
    
    # test kicker out
    test_kicker_out(kicker)
    
    # test kicker in
    test_kicker_in(kicker)
    
    # test kicker function
    test_kicker_fn(kicker) 
    
        