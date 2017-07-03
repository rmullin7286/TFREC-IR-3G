#pragma once

#include <Python.h>
#include <string>
#include <stdexcept>
#include <iostream>
#include <bcm2835.h>

using std::string;


/********************************************************************************
 * AUTHOR: RYAN MULLIN  https://github.com/rmullin7286
 * 
 * This is a C++ wrapper class for the Adafruit LCD shield for Raspberry pi.
 * For now, I'm only including the functions necessary for my own personal use,
 * but I may add more later if needed.
 * 
 * DEPENDENCIES: The Adafruit_CharLCD library from Adafruit and python 2.7
 * how to get dependencies: type the following commands into the terminal
 * 
 * sudo apt-get update
 * sudo apt-get install build-essential python-dev python-smbus python-pip
 * sudo pip install RPi.GPIO
 * cd (install directory)
 * sudo python setup.py install
 * ******************************************************************************/

enum class Button {NOBUTTON, SELECT, LEFT, UP, DOWN, RIGHT};

class LCDShield
{

public:
	LCDShield();
	~LCDShield();
	
	bool init();
	bool init_compatible();
	bool end();
	
	void print(const string &input);
	Button getButton();
	void isBacklit(bool backlit);
	


private:
	PyObject *pMain;
	PyObject *value;	
};
