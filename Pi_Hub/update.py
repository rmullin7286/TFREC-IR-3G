import sys
import os

os.chdir(os.path.dirname(os.path.realpath(__file__)) + '/../')
os.system('git pull')

os.chdir(os.path.dirname(os.path.realpath(__file__)))

if sys.argv[1] == 'PiSensor':
    os.system('g++ -std=c++11 -g ./src/main.cpp ./src/Adafruit_CharLCD_CPP.cpp ./src/PiSensor.cpp -o PiSensor -lbcm2835 -lrf24 -I/usr/include/python2.7 -lpython2.7')

elif sys.argv[1] == 'PiHub':
    os.system('g++ -std=c++11 -g ./src/main.cpp ./src/PiHub.cpp -o PiHub -lrf24 -lbcm2835')

else:
    os.system('sudo ./Pihub 1')
    exit(1)

os.system('sudo ./Pihub 0')
exit(0)


