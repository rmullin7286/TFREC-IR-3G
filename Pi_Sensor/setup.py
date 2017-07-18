#This is the setup script for the PiSensor application.

#HOW TO USE

#1
#2. change directory into the github folder for this program.
#3. enter the command "sudo python setup.py <directory>" where <directory> is the directory in which you want to install any dependency source files.

import os
import sys
import tarfile

#store the directory for the script. This is important later
scriptDir = os.path.dirname(os.path.abspath(__file__))

if not len(sys.argv) == 2:
    print('incorrect number of arguments.')
    sys.exit(1)
    
if not os.path.isdir(sys.argv[1]):
    print('Directory ' + sys.argv[1] + ' does not exist.')
    
dependencies = sys.argv[1] + '/TFREC-IR-3G-DEPENDENCIES'

os.mkdir(dependencies)

#cd to the directory of the file
os.chdir(dependencies)

#install bcm2835.h library
#status = os.system('wget http://www.airspayce.com/mikem/bcm2835/bcm2835-1.52.tar.gz ')
#if not os.WEXITSTATUS(status) == 0:
#    print('Error: could not locate package for bcm283.h. URL may have been moved.\n Please submit an issue at https://github.com/rmullin7286/TFREC-IR-3G/issues ')
#    sys.exit(1)

#tar = tarfile.open('bcm2835-1.52.tar.gz')
#tar.extractall()
#tar.close()
#os.chdir(dependencies + '/bcm2835-1.52')
#os.system('./configure')
#os.system('sudo make all')
#os.system('sudo make check')
#os.system('sudo make install')

#install the libraries for the adafruit char lcd plate
os.chdir(dependencies)
os.system('sudo apt-get -y install python-smbus i2c-tools')

os.system('sudo apt-get -y install build-essential python-dev python-smbus python-pip')
os.system('sudo pip install --no-input RPi.GPIO')
os.system('sudo apt-get -y install wiringpi')

status = os.system('git clone https://github.com/adafruit/Adafruit_Python_CharLCD.git')
if not os.WEXITSTATUS(status) == 0:
    print('git repo for Adafruit Char LCD plate could not be found. Please submit issue to https://github.com/rmullin7286/TFREC-IR-3G/issues')
    sys.exit(1)

os.chdir(dependencies + '/Adafruit_Python_CharLCD')
os.system('sudo python setup.py install')

#install RF24 librarys
os.chdir(dependencies)
status = os.system('git clone https://github.com/nRF24/RF24.git')
if not os.WEXITSTATUS(status) == 0:
    print('git repo for RF24 library could not be found. Please submit issue to https://github.com/rmullin7286/TFREC-IR-3G/issues')
    sys.exit(1)
    
os.chdir(dependencies + '/RF24')
os.system('sudo ./configure --driver=SPIDEV')
os.system('sudo make install -B')

#go back to the initial directory and compile the final program
os.chdir(scriptDir)

status = os.system('g++ -std=c++11 ./src/main.cpp ./src/Adafruit_CharLCD_CPP.cpp ./src/PiSensor.cpp -o PiSensor -lrf24 -lbcm2835 -I/usr/include/python2.7 -lpython2.7')
if not os.WEXITSTATUS(status) == 0:
    print('Error compiling: Please submit issue to https://github.com/rmullin7286/TFREC-IR-3G/issues')
    sys.exit(1)
    
print('PiSensor Program was installed succesfully!')
exit(0)
