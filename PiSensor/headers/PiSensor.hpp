#pragma once

#include "MLX90614.hpp"
#include "Adafruit_CharLCD_CPP.hpp"
#include <unistd.h>

class PiSensor
{
public:
	PiSensor();
	~PiSensor();
	
	void run();
	
private:
	void menu();
	void mainScreen();
	
	MLX90614 sensor;
	LCDShield shield;
	double ambient;
	double object;
	string print;
};
