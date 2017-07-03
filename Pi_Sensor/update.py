import sys
import os
import time

file = open(os.path.dirname(os.path.realpath(__file__)) + '/resources/version.txt', 'r')
buffer = file.readline()
buffer = file.readline()
numbers = buffer.split('.')

os.system('cd ' + os.path.dirname(os.path.realpath(__file__)) +  '/..')
os.system('git pull')

if sys.argv[1] == 'PiSensor':
    os.system('cd ' + os.path.dirname(os.path.realpath(__file)))
    os.system('g++ -std=c++11 -g ./src/main.cpp ./src/Adafruit_CharLCD_CPP.cpp ./src/PiSensor.cpp -o PiSensor -lbcm2835 -lrf24 -I/usr/include/python2.7 -lpython2.7')

elif sys.argv[1] == 'PiHub':
    os.system('cd ' + os.path.dirname(os.path.realpath(__file__)))
    os.system('g++ -std=c++11 -g ./src/main.cpp ./src/PiHub.cpp -o PiHub -lrf24 -lbcm2835')

else:
    exit(1)

file = open(os.path.dirname(os.path.realpath(__file__)) + '/resources/version.txt', 'r')
buffer = file.readline()
buffer = file.readline()
numbers_new = buffer.split()

i = 0
for line in numbers:
    if not line == numbers_new[i]:
        exit(0)
    i += 1

exit(1)


