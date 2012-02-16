
import os
import imp

try:
    installer = imp.load_source( 'install', 'C:\\WindRiver\\workspace\\robotpy\\utilities\\installer\\install.py' )
except IOError, e:
    print( "Error loading the RobotPy installer library. (%s)\n\n" % e +
           "-- Is RobotPy source installed at 'C:\\WindRiver\\workspace\\robotpy'?" )
    exit(1)
    
robot_host = '10.24.23.2'
    
print( 'Connecting to robot host %s' % robot_host )

try:
    server = installer.RobotCodeInstaller( robot_host )
except Exception as e:
    print( "Error connecting to remote host: %s" % e )
    exit(1)

local_root = os.path.dirname(__file__)


print( 'Beginning upload.' )

# verify the directories actually exist
server.create_remote_directory( '/py' )
server.delete_remote(           '/py/__pycache__' )
server.create_remote_directory( '/py/components' )
server.delete_remote(           '/py/components/__pycache__' )


# upload the files
files = [
    'robot.py',
    'robot_manager.py',
    'components/chamber.py',
    'components/feeder.py',
    'components/ramp_arm.py',
    'components/Shooter.py',
    'components/ShooterAngle.py',
    'components/ShooterSusan.py',
    'components/ShooterWheel.py',
]

for file in files:
    server.upload_file( remote_root, local_root, file )

print( 'Upload complete' )

# close the installer
server.close()
