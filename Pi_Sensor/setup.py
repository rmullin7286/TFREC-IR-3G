#This is the setup script for the python application

import os

#store the directory for the script
scriptDir = os.path.dirname(os.path.abspath(__file__))

#install apt-get libraries
os.system('sudo apt-get -y install python-smbus i2c-tools')
os.system('sudo apt-get -y install build-essential python-dev python-smbus python-pip')
os.system('sudo pip install --no-input RPi.GPIO')
os.system('sudo apt-get -y install wiringpi')

#setup adafruit char lcd
os.chdir(scriptDir + '/ext/Adafruit_Python_CharLCD')
os.system('sudo python setup.py install')

#setup bcm2835
os.chdir(scriptDir + '/ext/bcm2835-1.52')
os.system('./configure')
os.system('sudo make all')
os.system('sudo make install')
os.system('sudo make check')
os.system('sudo make install')

#setup rf24
os.chdir(scriptDir + '/ext/RF24')
os.system('sudo make all')
os.system('sudo make install')

#compile the final program
os.chdir(scriptDir)
status = os.system('g++ -std=c++11 ./src/main.cpp ./src/Adafruit_CharLCD_CPP.cpp ./src/PiSensor.cpp -o PiSensor -lrf24 -lbcm2835 -I/usr/include/python2.7 -lpython2.7')
if not os.WEXITSTATUS(status) == 0:
    print('Error compiling: Please submit issue to https://github.com/rmullin7286/TFREC-IR-3G/issues')
    sys.exit(1)
    
print('PiSensor Program was installed succesfully!')
exit(0)





