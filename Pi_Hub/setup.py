#Setup script for the Pi Hub app

import os
import sys

#check if running as root
if not os.geteuid() == 0:
    print("You need to run this script as root!\n")
    sys.exit(1)
    
#store the directory for the script
scriptDir = os.path.dirname(os.path.abspath(__file__))

#setup the RF24 library
os.chdir(scriptDir + '/ext/RF24')
os.system('make all')
os.system('make install')

#setup the conf file for sakis3G
print('sakis3G will be configured with this default configuration:\n\n'
      '--noprobe\n'
      '--pppd\n'
      '--console\n'
      'APN="CUSTOM_APN"\n'
      'CUSTOM_APN="Broadband"\n'
      'APN_USER="user"\n'
      'APN_PASS="pass"\n'
      'BAUD=115200\n'
      'MODEM="OTHER"\n'
      'OTHER="CUSTOM_TTY"\n'
      'CUSTOM_TTY="/dev/ttyAMA0"\n\n')

print('If you would like to change this, configure the file /etc/sakis3g.conf\n')

file = open('/etc/sakis3g.conf', 'w')
file.write('--noprobe\n'
      '--pppd\n'
      '--console\n'
      'APN="CUSTOM_APN"\n'
      'CUSTOM_APN="Broadband"\n'
      'APN_USER="user"\n'
      'APN_PASS="pass"\n'
      'BAUD=115200\n'
      'MODEM="OTHER"\n'
      'OTHER="CUSTOM_TTY"')

file.close()

#setup the dropbox uploader script

os.chdir(scriptDir)
os.system('./dropbox_uploader.sh')

#build the final program
os.system('g++ -std=c++11 ./src/main.cpp ./src/PiHub.cpp -o PiHub -lrf24')
