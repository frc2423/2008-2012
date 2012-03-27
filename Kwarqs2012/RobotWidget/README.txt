
RobotWidget 
-----------

The RobotWidget is an award-winning extension to the 2012 SmartDashboard (Team 
2423 won the Innovation in Controls award at the Boston Regional, and this was
cited as a key component of that). It is based in part off of the 
SendablePIDController and Image widgets that come with the SmartDashboard. 

On our robot, this widget shows a picture of the robot, and when the ball 
sensors on our robot senses a ball it tells the widget to display a picture
of a basketball on the appropriate place in our robot. 

How it actually works is quite simple:

    - The widget always displays robot.png 
    - If the robot sets a variable on a network table to True, then it 
    displays the image associated with that variable on top of the background
    image. The images are all png format, and are mostly transparent so that
    multiple images can be displayed simulataneously. 

Currently, this widget is very specific to our robot, but I plan to generalize
it at some point and submit it to the SmartDashboard project to be included
with the default SmartDashboard. 

Feel free to use and modify this as you wish subject to the terms in LICENSE, 
just be sure to give our team credit! :) 

The included images were created by Linda Donoghue. 

Dustin Spicuzza <dustin@virtualroadside.com>
FIRST Team 2423


PC Installation instructions: 

    - dist\RobotWidget.jar is copied to $(SmartDashboard)\extensions\
    - img\*.png is copied to $(SmartDashboard)\extensions\img
    
    It will show up in your SmartDashboard when you create an instance 
    of the remote RobotWidget type on the cRio.
    
Robot installation instructions (Python/RobotPy):

    Included in robot\robot_widget.py is a python module that should be
    copied to the robot, and then imported from robot.py using something
    similar to this:
    
        from robot_widget import RobotWidget
        
        # creation of the type will automatically add it to the SmartDashboard
        # using the name specified in the constructor, so you don't need to
        # call PutData() on it
        widget = RobotWidget('Widget')
    

Robot installation instructions (Other languages):

    The amount of code necessary to make this work is very minimal, but
    as we don't use the other languages I haven't taken the time to do it. 
    Look at the python module to see how it is implemented, and do the 
    same thing for other languages. 
    
    Note that you will need to use PutBoolean instead of the dictionary
    interface that python uses, since this is a RobotPy-specific feature.
