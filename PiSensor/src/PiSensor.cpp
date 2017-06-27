#include "../headers/PiSensor.hpp"

PiSensor::PiSensor()
{
	shield.init_compatible();
	shield.print("   Hello!");
	sleep(2);
}

PiSensor::~PiSensor()
{
	shield.end();
}

void PiSensor::mainScreen()
{
	while(1)
	{
		//check to see if the user has pressed a button.
		//If any button has been pressed, initiate the menu.
		if(shield.getButton() != Button::NOBUTTON) menu();
		
		object = sensor.readObject();
		ambient = sensor.readAmbient();
	}
}

void PiSensor::menu()
{
	
}

void PiSensor::run()
{
	mainScreen();
}
