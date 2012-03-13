
import os
from glob import glob
import imp
import inspect

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
    
    def __init__(self, ds, drive, ramp_arm, ball_handler, shooter, robot_manager):
        
        self.ds = ds
        self.modes = {}
        self.active_mode = None
        
        print( "AutonomousModeManager::__init__() Begins" )
        
        # load all modules in the current directory
        modules_path = os.path.dirname(os.path.abspath(__file__))
        #modules = glob( os.path.join( modules_path, '*.py' ) )
        modules = []
        
        for module_filename in modules:
            
            module_name = os.path.basename(module_filename[:-3])
            
            if module_name == '__init__':
                continue
        
            try:
                __import__( module_name )
                #module = imp.load_source( module_name, module_filename )
            except:
                if not self.ds.IsFMSAttached():
                    raise
            
            #
            # Find autonomous mode classes in the modules that are present
            # -> note that we actually create the instance of the objects here,
            #    so that way we find out about any errors *before* we get out 
            #    on the field.. 
            
            for name, obj in inspect.getmembers( module, inspect.isclass ):
                if issubclass( obj, AutonomousModeBase ) and obj != AutonomousModeBase:
                    try:
                        instance = obj( drive, ramp_arm, ball_handler, shooter, robot_manager )
                    except:
                        if not self.ds.IsFMSAttached():
                            raise
                        else:
                            continue
                    
                    if hasattr( instance, 'NAME' ):
                        if instance.NAME in self.modes:
                            if not self.ds.IsFMSAttached():
                                raise RuntimeError( "Duplicate name %s in %s" % (instance.NAME, module_filename) )
                            
                            print( "ERROR: Duplicate name %s specified by object type %s in module %s" % (instance.NAME, name, module_filename))
                            self.modes[ name + '_' + module_filename ] = instance
                        else:
                            self.modes[ instance.NAME ] = instance
        
        # now that we have a bunch of valid autonomous mode objects, let 
        # the user select one using the SmartDashboard.
        for k,v in self.modes.items():
            # TODO
            pass
        
        # TODO: The smart dashboard lets us save the table values
        # somewhere... let's do that if the value changes, so that
        # we have a sensible default
        
        print( "AutonomousModeManager::__init__() Done" )
        
        
    def OnAutonomousEnable(self):
        # select the active autonomous mode here, and enable it
        
        self.active_mode = self.modes[key]
        
        
 
    def OnAutonomousDisable(self):
        if self.active_mode is not None:
            self.active_mode.OnDisable()
        
    def Update(self, time_elapsed):
        if self.active_mode is not None:
            self.active_mode.Update( time_elapsed )
    

