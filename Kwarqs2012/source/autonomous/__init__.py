

from glob import glob
import imp
import inspect
import os
import sys

try:
    import wpilib
    import wpilib.SmartDashboard
except ImportError:
    import fake_wpilib as wpilib
    import fake_wpilib.SmartDashboard


class AutonomousModeBase(object):
    '''
        Base class for autonomous modes
    '''
    
    def OnEnable(self):
        print( "AutonomousModeBase::OnEnable: override me" )
        
    def OnDisable(self):
        print( "AutonomousModeBase::OnDisable: override me" )

    def Update(self):
        print( "AutonomousModeBase::Update: override me" )
    


class AutonomousModeManager(object):
    '''
        The autonomous manager loads all autonomous mode modules and allows
        the user to select one of them via the SmartDashboard. 
        
        See template.txt for a sample autonomous mode module
    '''
    
    def __init__(self, drive, ramp_arm, ball_handler, robot_manager):
        
        self.ds = wpilib.DriverStation.GetInstance()
        self.modes = {}
        self.active_mode = None
        
        print( "AutonomousModeManager::__init__() Begins" )
        
        # load all modules in the current directory
        modules_path = os.path.dirname(os.path.abspath(__file__))
        sys.path.append( modules_path )
        modules = glob( os.path.join( modules_path, '*.py' ) )
        
        for module_filename in modules:
            
            module_name = os.path.basename(module_filename[:-3])
            
            if module_name in  ['__init__', 'manager']:
                continue
        
            try:
                module = imp.load_source( module_name, module_filename )
            except:
                if not self.ds.IsFMSAttached():
                    raise
            
            #
            # Find autonomous mode classes in the modules that are present
            # -> note that we actually create the instance of the objects here,
            #    so that way we find out about any errors *before* we get out 
            #    on the field.. 
            
            for name, obj in inspect.getmembers( module, inspect.isclass ):

                if hasattr( obj, 'MODE_NAME') :
                    try:
                        instance = obj( drive, ramp_arm, ball_handler, robot_manager )
                    except:
                        
                        if not self.ds.IsFMSAttached():
                            raise
                        else:
                            continue
                    
                    if instance.MODE_NAME in self.modes:
                        if not self.ds.IsFMSAttached():
                            raise RuntimeError( "Duplicate name %s in %s" % (instance.MODE_NAME, module_filename) )
                        
                        print( "ERROR: Duplicate name %s specified by object type %s in module %s" % (instance.MODE_NAME, name, module_filename))
                        self.modes[ name + '_' + module_filename ] = instance
                    else:
                        self.modes[ instance.MODE_NAME ] = instance
        
        # now that we have a bunch of valid autonomous mode objects, let 
        # the user select one using the SmartDashboard.
        
        # SmartDashboard interface
        sd = wpilib.SmartDashboard.SmartDashboard.GetInstance()
        self.chooser = wpilib.SmartDashboard.SendableChooser()
        sd.PutData( 'Autonomous Mode', self.chooser ) 
        
        print("Loaded autonomous modes:")
        for k,v in self.modes.items():
            
            if hasattr(v, 'DEFAULT') and v.DEFAULT == True:
                print( " -> %s [Default]" % k )
                self.chooser.AddDefault( k, v )
            else:
                print( " -> %s" % k )
                self.chooser.AddObject( k, v )
        
        
        print( "AutonomousModeManager::__init__() Done" )
        
        
    def OnAutonomousEnable(self):
        '''Select the active autonomous mode here, and enable it'''
        self.active_mode = self.chooser.GetSelected()
        if self.active_mode is not None:
            print( "AutonomousModeManager: Enabling %s" % self.active_mode.MODE_NAME )
            self.active_mode.OnEnable()
 
    def OnAutonomousDisable(self):
        '''Disable the active autonomous mode'''
        if self.active_mode is not None:
            print( "AutonomousModeManager: Disabling %s" % self.active_mode.MODE_NAME )
            self.active_mode.OnDisable()
            
        self.active_mode = None
        
    def Update(self, time_elapsed):
        if self.active_mode is not None:
            self.active_mode.Update( time_elapsed )

