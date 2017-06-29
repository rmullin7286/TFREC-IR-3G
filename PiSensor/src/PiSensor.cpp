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
	
	save.open("signature.txt", fstream::in);
	
	if(save.good()) getline(save, signature);
	else signature = "nosig";
	
	save.close();
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
		double ambient = sensor.readAmbient();
		double object = sensor.readObject();
		
		//check to see if the user has pressed a button.
		//If any button has been pressed, initiate the menu.
		if(shield.getButton() != Button::NOBUTTON) menu(ambient, object);
		
		format << "AMBIENT: " << ambient << "\\nOBJECT: " << object;
		
		shield.print(format.str());
		format.str("");
		
		sleep(1);
	}
}

void PiSensor::menu(double ambient, double object)
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
		usleep(3);
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
			usleep(3);
			
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
			case MenuItem::LOG: log(ambient, object);
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
			usleep(3);
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
	
	//send a packet to the hub requesting a test
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
			radio.read(&status, sizeof(int));
			break;
		}
	}
	
	if(!status) shield.print("Error: No radio\\nresponse.");
	
	else
	{
		shield.print("Testing\\nconnection");
		
		//wait for success status after hub has connected.
		start = time(NULL);
		end = time(NULL);
		status = 0;
	
		//status -1 = ping unsuccesful
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
		
		if(status == -1) shield.print("Error: test\\nunsuccesful");
		
		else if(status == 0) shield.print("Error: radio\\ntimeout.");
		
		else shield.print("Sensor is\\nconnected!");
		
	}
	
	radio.stopListening();
	
	sleep(2);
}

void PiSensor::log(double ambient, double object)
{
	Packet send;
	send.flag = PacketFlag::LOG;
	send.ambient = ambient;
	send.object = object;
	
	time_t start;
	time_t end;
	
	int status;
	
	shield.print("Sending \\nrequest");
	
	//send a packet to the hub requesting 
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
			radio.read(&status, sizeof(int));
			break;
		}
	}
	
	if(!status) shield.print("Error: No radio\\nresponse.");
	
	else
	{
		shield.print("logging measurement:");
	}
}

void PiSensor::upload()
{
	
}

void PiSensor::setSignature()
{
	Button input;
	Button release;
	int cursor;
	string display;
	char sig[16] = "\0";
	bool done = false;
	int length;
	
	
	//load the ASCII values into an array; (32 space, 97-122 alphabet, 48-57 numbers)
	char characters[37];
	characters[0] = 32;
	
	for (int i = 1; i < 27; i++)
	{
		characters[i] = i + 96;
	}
	
	for (int i = 27; i < 37; i++)
	{
		characters[i] = i + 21;
	}
	
	sig[0] = characters[1];
	length = 1;
	display = "Enter signature:\\n";
	
	while(!done)
	{
		shield.print(display + sig);
		
		do
		{
			input = shield.getButton();
			usleep(2);
		}while(input == Button::NOBUTTON);
		
		do
		{
			release = shield.getButton();
			usleep(2);
		}while(release != Button::NOBUTTON);
		
		switch(input)
		{
		case Button::SELECT: done = true;
			break;
		case Button::LEFT: if(length > 1) { sig[length - 1] = '\0';
			length--; }
			break;
		case Button::RIGHT: if(length < 16) { length++;
			sig[length - 1] = characters[1]; }
			break;
		case Button::DOWN: if(sig[length - 1] == characters[0]) sig[length - 1] = characters[36];
			else if(sig[length - 1] == characters[1]) sig[length - 1] = characters[0];
			else if(sig[length - 1] == characters[27]) sig[length - 1] = characters[26];
			else sig[length - 1]--;
			break;
		case Button::UP: if(sig[length - 1] == characters[36]) sig[length - 1] = characters[0];
			else if(sig[length - 1] == characters[26]) sig[length - 1] = characters[27];
			else if(sig[length - 1] == characters[0]) sig[length - 1] = characters[1];
			else sig[length - 1]++;
			break;
		}
	}
	
	display = "Signature set:\\n";
	signature = sig;
	shield.print(display + signature);
	
	//save the signature to a txt file
	save.open("signature.txt", fstream::out);
	save << signature << std::endl;
	save.close();
	
	sleep(2);
}

void PiSensor::disconnect()
{
	
}

void PiSensor::shutdown()
{
	
}
