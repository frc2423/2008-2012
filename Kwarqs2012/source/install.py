
import os
import imp
import msvcrt
import subprocess

#
# simple test before sending it out, in case of simple errors
#

print( 'Importing robot.py...' )

if os.name == 'nt':
    ret = subprocess.call( os.path.normpath( os.path.dirname(__file__) + '/../testing/test.bat --import .' ), shell=True )
else:
    ret = subprocess.call( os.path.normpath( os.path.dirname(__file__) + '/../testing/test.sh --import .' ), shell=True )
    
if ret != 0:
    print( "Failure detected, aborting import" )
    exit(1)
    

print( 'Import successful!' )


#
# Ok, load the installer now
#


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


# wipe the whole thing, to ensure that we don't have any cruft lying around
server.delete_remote(           '/py' )
server.create_remote_directory( '/py' )

# upload the files
files = [
    'controls.py',
    'operator_leds.py',
    'robot.py',
    'robot_widget.py',
    'util.py'
]

dirs = [
    'autonomous',
    'components'
]

# restore boot.py
server.upload_file( remote_root, 'C:\\WindRiver\\workspace\\robotpy\\py', 'boot.py' )

for file in files:
    server.upload_file( remote_root, local_root, file )
    
for dir in dirs:
    server.upload_directory( remote_root + '/' + dir, local_root + '/' + dir, verbose=True )

print( 'Upload complete. Press any key to continue.' )

# close the installer
server.close()
msvcrt.getch()
