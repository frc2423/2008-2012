
import os
import imp
import msvcrt

try:
    installer = imp.load_source( 'install', 'C:\\WindRiver\\workspace\\robotpy\\utilities\\installer\\install.py' )
except IOError, e:
    print( "Error loading the RobotPy installer library. (%s)\n\n" % e +
           "-- Is RobotPy source installed at 'C:\\WindRiver\\workspace\\robotpy'?" )
    msvcrt.getch()
    exit(1)
    
robot_host = '10.24.23.2'
    
print( 'Connecting to robot host %s' % robot_host )

try:
    server = installer.RobotCodeInstaller( robot_host )
except Exception as e:
    print( "Error connecting to remote host: %s" % e )
    msvcrt.getch()
    exit(1)

local_root = os.path.dirname(__file__)


print( 'Beginning upload.' )

remote_root = '/py'

# verify the directories actually exist
server.create_remote_directory( '/py' )
server.delete_remote(           '/py/__pycache__' )
server.create_remote_directory( '/py/components' )
server.delete_remote(           '/py/components/__pycache__' )
server.create_remote_directory( '/py/components/shooter' )
server.delete_remote(           '/py/components/shooter/__pycache__' )


# upload the files
# -> local root, remote file name
files = [
    ( local_root, 'robot.py' ), 
    ('../source/', 'operator_leds.py')
]

for file in files:
    server.upload_file( remote_root, file[0], file[1] )

print( 'Upload complete' )

# close the installer
server.close()
msvcrt.getch()
