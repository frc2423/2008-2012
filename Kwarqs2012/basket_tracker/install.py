
import os
import imp

install = imp.load_source( 'install', 'C:\\WindRiver\\workspace\\robotpy\\utilities\\installer\\install.py' )

installer = install.RobotCodeInstaller( '10.24.23.2' )

local_root = os.path.dirname(__file__)
remote_root = '/lib/python3.2/site-packages'

print( 'Beginning upload of basket tracker' )

installer.upload_file( local_root, remote_root, '_basket_tracker.pyd' )
installer.upload_file( local_root, remote_root, 'basket_tracker.py' )
installer.close()
