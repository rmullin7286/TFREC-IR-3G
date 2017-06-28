#include "../headers/PiSensor.hpp"

PiSensor::PiSensor(uint16_t gpio, uint16_t ce) : radio(gpio, ce), pipes{"1Node", "2Node"}
{
	shield.init_compatible();
	shield.print("     Hello!");
	radio.begin();
	radio.setRetries(15,15);
	radio.openWritingPipe(pipes[1]);
	radio.openReadingPipe(1, pipes[0]);
	radio.setPALevel(RF24_PA_MAX);
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
		
		format << "AMBIENT: " << sensor.readAmbient() << "\\nOBJECT: " << sensor.readObject();
		
		shield.print(format.str());
		format.str("");
		
		sleep(1);
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
	bool back = false;
	
	shield.print("    MENU   ");
	
	//wait for the user to release the button
	do
	{
		input = shield.getButton();
	}while(input != Button::NOBUTTON);
	
	while(!back)
	{
		//print the current MenuItem onto the screen
		switch(item)
		{
		case MenuItem::CONNECT:    shield.print(" <  CONNECT   >");
				break;
		case MenuItem::TEST:       shield.print(" <    TEST    >");
				break;
		case MenuItem::LOG:        shield.print(" <    LOG     >");
				break;
		case MenuItem::UPLOAD:     shield.print(" <   UPLOAD   >");
				break;
		case MenuItem::SET_SIG:    shield.print(" <  SET SIG   >");
				break;
		case MenuItem::DISCONNECT: shield.print(" < DISCONNECT >");
				break;
		case MenuItem::SHUTDOWN:   shield.print(" <  SHUTDOWN  >");
				break;
		case MenuItem::BACK:       shield.print(" <    BACK    >");
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
			case MenuItem::BACK: back = true;
				break;
			}
			break;
		}
		
		//do another do loop to wait until the user releases the button so that the menu doesn't keep scrolling
		do
		{
			input = shield.getButton();
		}while(input != Button::NOBUTTON);
	}
}

void PiSensor::run()
{
	mainScreen();
}

void PiSensor::connect()
{
	Packet send;
	int status;
	time_t start, end;
	double elapsed;
	bool read;
	bool success = false;
	
	//prepare the packet
	send.ambient = 0.0;
	send.object = 0.0;
	send.flag = PacketFlag::CONNECT;
	send.signature[0] = '\0';
	
	shield.print("Sending\\nrequest");
	
	//send a packet to the hub requesting a connection
	radio.write(&send, sizeof(Packet));
	
	//wait for an acknowledgement response
	radio.startListening();
	
	start = time(NULL);
	end = time(NULL);
	status = 0;
	
	while(difftime(end, start) < 5.0)
	{
		end = time(NULL);
		
		if(radio.available())
		{
			radio.read(&success, sizeof(bool));
			break;
		}
	}
	
	if(!success) shield.print("Error: No radio\\nresponse.");
	
	else
	{
		shield.print("Connecting to\\ncell service");
		
		//wait for success status after hub has connected.
		start = time(NULL);
		end = time(NULL);
		status = 0;
	
		//status -1 = Error
		//status 0 = no send back
		//status 1 = Success
		
		while(difftime(end, start) < 30.0)
		{
			end = time(NULL);
			
			if(radio.available())
			{
				radio.read(&status, sizeof(int));
				read = true;
				break;
			}
		}
		
		if(status == -1) shield.print("Error: Could not\\nnot connect.");
		
		else if(status == 0) shield.print("Error: radio\\ntimeout.");
		
		else shield.print("Connected\\nsuccesfully!");
		
	}
	
	sleep(2);
	radio.stopListening();
}

void PiSensor::test()
{
	Packet send;
	int status;
	time_t start, end;
	double elapsed;
	bool read;
	
	//prepare the packet
	send.ambient = 0.0;
	send.object = 0.0;
	send.flag = PacketFlag::TEST;
	send.signature[0] = '\0';
	
	//send a packet to the hub requesting a connection
	radio.write(&send, sizeof(Packet));
	
	//wait for an acknowledgement response
	radio.startListening();
	
	start = time(NULL);
	end = time(NULL);
	status = 0;
	
	while(difftime(end, start) < 5.0)
	{
		if(radio.available())
		{
			radio.read(&status, sizeof(int));
			break;
		}
	}
	
	if(!status) shield.print("Error: No radio\\nresponse.");
	
	else
	{
		//wait for success status after hub has connected.
		start = time(NULL);
		end = time(NULL);
		status = 0;
	
		//status -1 = ping unsuccesful
		//status 0 = no send back
		//status 1 = Success
		
		while(difftime(end, start) < 60.0)
		{
			if(radio.available())
			{
				radio.read(&status, sizeof(int));
				read = true;
				break;
			}
		}
		
		if(status == -1) shield.print("Error: test\\nunsuccesful");
		
		else if(status == 0) shield.print("Error: radio\\ntimeout.");
		
		else shield.print("Sensor is\\nconnected!");
		
	}
	
	radio.stopListening();
	
	sleep(2);
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
