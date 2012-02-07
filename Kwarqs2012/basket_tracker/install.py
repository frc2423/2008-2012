
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
remote_root = '/lib/python3.2/site-packages'

print( 'Beginning upload of basket tracker' )

# verify the directories actually exist
server.create_remote_directory( remote_root )
server.delete_remote( remote_root + '/__pycache__' )

# upload the files
server.upload_file( remote_root, local_root, '_basket_tracker.pyd' )
server.upload_file( remote_root, local_root, 'basket_tracker.py' )

print( 'Upload complete' )

# close the installer
server.close()
