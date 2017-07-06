import sys
import os

filePath = os.path.dirname(os.path.realpath(__file__))
os.chdir(filePath + '/../')
os.system('git pull')

os.chdir(filePath)

if sys.argv[1] == 'PiSensor':
    os.system('g++ -std=c++11 -g ./src/main.cpp ./src/Adafruit_CharLCD_CPP.cpp ./src/PiSensor.cpp -o PiSensor -lbcm2835 -lrf24 -I/usr/include/python2.7 -lpython2.7')
    os.system('sudo ./PiSensor')

elif sys.argv[1] == 'PiHub':
    os.system('g++ -std=c++11 -g ./src/main.cpp ./src/PiHub.cpp -o PiHub -lrf24 -lbcm2835')
    os.system('sudo ./PiHub')




