'''
    Test harness to be used with fake_wpilib. Ideally, this should be
    generic enough to test any robot code you throw at it.
    
    How it works:
    
        This base test program doesn't really have any intelligence, it
        just loads robot code from a specified location and then runs
        code in a separately defined test module. 
        
    Implementing your own tests:
        
        In the 'tests' directory, add your own testname.py file
        
        In the module, there should be two things:
            - a variable called 'robot_path', which has the path of your
            robot code, relative to the directory your tests are located
            - A function called 'run_tests', which takes two arguments:
                - module:   The module for your robot code
                - myrobot:  The robot class returned by your run() function
        
    Running your tests:
    
        test.bat testname
        test.sh testname
        
        .. or some variation thereof. Don't run test.py directly, otherwise
        the pycache will be generated, which we don't want.
        
    TODO:
    
        - Need to make creating tests easier, add a template, base class,
        or something that makes the logic a lot easier. 
        
        - Fake wpilib needs a better way to access the raw objects, instead
        of traversing through the robot module to get at them. 
        
    
'''

from glob import glob
import imp
import os
from optparse import OptionParser
import sys

import fake_wpilib as wpilib

testdir_path = os.path.dirname(os.path.abspath(__file__))
modules_path = os.path.join( testdir_path, 'tests' )
modules = [ os.path.basename(module[:-3]) for module in glob( os.path.join( modules_path, '*.py' ) ) ]


if __name__ == '__main__':

    parser = OptionParser()
    
    parser.add_option(  '--fms',
                        action='store_true', dest='fms_attached', default=False,
                        help='Pretend that the robot is on the field' )
                        
    (options, args) = parser.parse_args()
    
    if len(args) != 1:
        sys.stderr.write( "You must specify a test module to run. Available test modules:\n" )
        
        for module in modules:
            sys.stderr.write( "  %s\n" % module )
            
        sys.stderr.write( '\n' )

        exit(1)
    
    test_module_name = args[0]
    
    if test_module_name not in modules:
        sys.stderr.write( "Invalid module name \"%s\"\n" % test_module_name )
        exit(1)
        
    # setup options
    ds = wpilib.DriverStation.GetInstance()
    ds.fms_attached = options.fms_attached

    # import the test module
    test_module = imp.load_source( test_module_name, os.path.join( modules_path, test_module_name + '.py' ))
    
    # add the robot directory to the path, so it can load other things
    if not hasattr( test_module, 'robot_path' ):
        sys.stderr.write( "ERROR: the test module '%s' does not have a 'robot_path' global variable\n" % test_module_name )
        exit(1)
    
    # convert \ to / or whatever, depending on the platform
    test_module.robot_path = os.path.normpath( test_module.robot_path )
    sys.path.append( os.path.join( modules_path, test_module.robot_path ) )
    
    # setup the robot code
    import robot

    myrobot = wpilib.initialize_robot()
    if myrobot is None:
        sys.stderr.write( "ERROR: the run() function in robot.py MUST return an instance of your robot class\n" )
        exit(1)
    
    #
    # Finally, run the tests
    #
    
    test_module.run_tests( robot, myrobot )
    
    print( "Test complete." )
    
    
    
    
    

