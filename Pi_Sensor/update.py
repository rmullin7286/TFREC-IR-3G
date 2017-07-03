import sys
import os

file = open('./resources/version.txt', 'r')
buffer = file.readline()
buffer = file.readline()
numbers = buffer.split('.')

os.system('cd ..')
os.system('git pull')

if sys.argv[1] == 'PiSensor':
    os.system('cd Pi_Sensor')
    os.system('g++ -std=c++11 -g ./src/main.cpp ./src/Adafruit_CharLCD_CPP.cpp ./src/PiSensor.cpp -o PiSensor -lbcm2835 -lrf24 -I/usr/include/python2.7 -lpython2.7')

elif sys.argv[1] == 'PiHub':
    os.system('cd Pi_Hub')
    os.system('g++ -std=c++11 -g ./src/main.cpp ./src/PiHub.cpp -o PiHub -lrf24 -lbcm2835')

else:
    exit(1)

file = open('./resources/version.txt', 'r')
buffer = file.readline()
buffer = file.readline()
numbers_new = buffer.split()

i = 0
for line in numbers:
    if line != numbers_new[i]:
        exit(0)
    i += 1

exit(1)


