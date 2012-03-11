
import os
import imp
import msvcrt

#
# simple test before sending it out, in case of simple errors
#

print( 'Importing robot.py...' )

test_path = os.path.normpath( os.path.dirname(__file__) + '/../testing/test.py' ) 
tester = imp.load_source( 'test', test_path )
tester.import_robot( os.path.normpath( os.path.dirname(__file__) + 'robot.py' ) )

print( 'Import successful!' )


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
files = [
    'robot.py',
    'util.py'
]

for file in files:
    server.upload_file( remote_root, local_root, file )

print( 'Upload complete' )

# close the installer
server.close()
msvcrt.getch()
