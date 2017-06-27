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
	ostringstream format;
	
	while(1)
	{
		//check to see if the user has pressed a button.
		//If any button has been pressed, initiate the menu.
		if(shield.getButton() != Button::NOBUTTON) menu();
		
		format << "AMBIENT: " << sensor.readAmbient() << "\nOBJECT: " << sensor.readObject();
		
		shield.print(format.str());
		format.str("");
	}
}

void PiSensor::menu()
{
	//CONNECT = 0 
	//TEST = 1
	//LOG = 2
	//UPLOAD = 3
	//SET_SIG = 4
	//DISCONNECT = 5
	//SHUTDOWN = 6
	//BACK = 7
	Button input;
	MenuItem item = MenuItem::CONNECT;
	
	do
	{
		//print the current MenuItem onto the screen
		switch(item)
		{
		case MenuItem::CONNECT:    shield.print("<  CONNECT   >");
				break;
		case MenuItem::TEST:       shield.print("<    TEST    >");
				break;
		case MenuItem::LOG:        shield.print("<    LOG     >");
				break;
		case MenuItem::UPLOAD:     shield.print("<   UPLOAD   >");
				break;
		case MenuItem::SET_SIG:    shield.print("<  SET SIG   >");
				break;
		case MenuItem::DISCONNECT: shield.print("< DISCONNECT >");
				break;
		case MenuItem::SHUTDOWN:   shield.print("<  SHUTDOWN  >");
				break;
		case MenuItem::BACK:       shield.print("<    BACK    >");
				break;
		}
		
		//wait for left or right input
		do
		{
			input = shield.getButton();
			
		}while(input != Button::LEFT && input != Button::RIGHT && input != Button::SELECT);
	
		switch(input)
		{
		case Button::LEFT: static_cast<int>(item) == 0 ? item = MenuItem::BACK : item = static_cast<MenuItem>(static_cast<int>(item) - 1);
			break;
		case Button::RIGHT: static_cast<int>(item) == 7 ? item = MenuItem::CONNECT : item = static_cast<MenuItem>(static_cast<int>(item) + 1);
			break;
		case Button::SELECT:
			switch(item)
			{
			case MenuItem::CONNECT: connect();
				break;
			case MenuItem::TEST: test();
				break;
			case MenuItem::LOG: log();
				break;
			case MenuItem::UPLOAD: upload();
				break;
			case MenuItem::SET_SIG: setSignature();
				break;
			case MenuItem::DISCONNECT: disconnect();
				break;
			case MenuItem::SHUTDOWN: shutdown();
				break;
			}
			break;
		}
		
	}while(item != MenuItem::BACK);
}

void PiSensor::run()
{
	mainScreen();
}

void PiSensor::connect()
{
	
}

void PiSensor::test()
{
	
}

void PiSensor::log()
{
	
}

void PiSensor::upload()
{
	
}

void PiSensor::setSignature()
{
	
}

void PiSensor::disconnect()
{
	
}

void PiSensor::shutdown()
{
	
}
