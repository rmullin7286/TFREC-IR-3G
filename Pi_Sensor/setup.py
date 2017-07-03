#This is the setup script for the PiSensor application.

#HOW TO USE

#1
#2. change directory into the github folder for this program.
#3. enter the command "sudo python setup.py <directory>" where <directory> is the directory in which you want to install any dependency source files.

import os
import sys

#store the directory for the script. This is important later
scriptDir = os.path.dirname(os.path.abspath(__file__))

if not os.geteuid() == 0:
    print('setup.py must be run as root. use command "sudo python setup.py <directory"')
    sys.exit(1)

if not len(sys.argv) == 2:
    print('incorrect number of arguments.')
    sys.exit(1)

#cd to the directory of the file
cd = 'cd' + sys.argv[1]
status = os.system(cd)
if os.WEXITSTATUS(status) == 1:
    print('Error: directory ' + sys.argv[1] + ' does not exist')
    sys.exit(1)

#install bcm2835.h library
status = os.system('wget http://www.airspayce.com/mikem/bcm2835/bcm2835-1.52.tar.gz ')
if not os.WEXITSTATUS(status) == 0:
    print('Error: could not locate package for bcm283.h. URL may have been moved.\n Please submit an issue at https://github.com/rmullin7286/TFREC-IR-3G/issues ')
    sys.exit(1)

os.system('tar zxvf bcm2835-1.52.tar.gz')
os.system('cd bcm2835-1.xx')
os.system('./configure')
os.system('make')
os.system('make check')
os.system('make install')

#install RF24 librarys
os.system('cd ..')
status = os.system('git clone https://github.com/nRF24/RF24.git')
if not os.WEXITSTATUS(status) == 0:
    print('git repo for RF24 library could not be found. Please submit issue to https://github.com/rmullin7286/TFREC-IR-3G/issues')
    sys.exit(1)

os.system('cd RF24')
os.system('make all')
os.system('make install')

#install the libraries for the adafruit char lcd plate
os.system('cd ..')
os.system('apt-get -y install python-smbus i2c-tools')

os.system('apt-get update')
os.system('apt-get install build-essential python-dev python-smbus python-pip')
os.system('sudo pip install RPi.GPIO')
os.system('apt-get install wiringpi')

status = os.system('git clone https://github.com/adafruit/Adafruit_Python_CharLCD.git')
if not os.WEXITSTATUS(status) == 0:
    print('git repo for Adafruit Char LCD plate could not be found. Please submit issue to https://github.com/rmullin7286/TFREC-IR-3G/issues')
    sys.exit(1)

os.system('cd Adafruit_Python_CharLCD')
os.system('sudo python setup.py install')

#go back to the initial directory and compile the final program
os.system('cd ' + scriptDir)
os.system('cd ./PiSensor')

status = os.system('g++ -std=c++11 ./src/main.cpp ./src/Adafruit)CharLCD_CPP.cpp ./src/PiSensor.cpp -o PiSensor -lbcm2835 -lrf24 -I/usr/include/python2.7 -lpython2.7')
if not os.WEXITSTATUS(status) == 0:
    print('Error compiling: Please submit issue to https://github.com/rmullin7286/TFREC-IR-3G/issues')




