#include "../headers/PiSensor.hpp"

PiSensor::PiSensor(uint16_t gpio, uint16_t ce) : radio(gpio, ce), pipes{"1Node", "2Node"}
{
	shield.init_compatible();
	shield.print("     Hello!");
	radio.begin();
	radio.setAutoAck(true);
	radio.setRetries(15,15);
	radio.openWritingPipe(pipes[1]);
	radio.openReadingPipe(1, pipes[0]);
	radio.setPALevel(RF24_PA_LOW);
	radio.setDataRate(RF24_250KBPS);
	//radio.setCRCLength(RF24_CRC_8);
	//radio.setPayloadSize(sizeof(Packet));
	radio.printDetails();
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
		
		format << std::fixed << "AMBIENT: " << std::setprecision(2) << ambient << "\\nOBJECT: " << std::setprecision(2) << object;
		
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
	//UPDATE = 7
	//BACK = 8
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
		case MenuItem::UPDATE:     shield.print(" <   UPDATE   >");
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
		case Button::RIGHT: static_cast<int>(item) == 8 ? item = MenuItem::CONNECT : item = static_cast<MenuItem>(static_cast<int>(item) + 1);
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
			case MenuItem::UPDATE: update();
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
	int status = 0;
	time_t start, end;
	
	//prepare the packet
	send.ambient = 0.0;
	send.object = 0.0;
	send.flag = PacketFlag::CONNECT;
	send.signature[0] = '\0';
	
	shield.print("Sending\\nrequest");
	
	//send a packet to the hub requesting a connection.
	//Repeat 3 times. If it doesn't go through by then, exit the function.
	for(int i = 0; i < 3; i++)
	{
		if(radio.write(&send, sizeof(Packet)))
		{
			status = 1;
			break;
		}
		
		shield.print("Request not\\nsent. Retrying.");
	}
	
	if(status == 0) shield.print("Error: Radio\\ntimeout");
	
	else
	{
		shield.print("Connecting to\\ncell service");
		
		//wait for success status after hub has connected.
		start = time(NULL);
		end = time(NULL);
		status = 1;
		
		while(difftime(end, start) < 30.0)
		{
			end = time(NULL);
			
			if(radio.available())
			{
				radio.read(&status, sizeof(int));
				break;
			}
			
			usleep(2);
		}
		
		radio.stopListening();
		
		if(status == 1) shield.print("Error: Could not\\nnot connect.");
		
		else if(status == 0) shield.print("Connected\\nsuccesfully!");
		
		else shield.print("Error: Radio\\ntimeout");
	}
	
	sleep(2);
}

void PiSensor::test()
{
	Packet send;
	int status = 0;
	time_t start, end;
	
	//prepare the packet
	send.ambient = 0.0;
	send.object = 0.0;
	send.flag = PacketFlag::TEST;
	send.signature[0] = '\0';
	
	//send a packet to the hub requesting a test
	//Retry 3 times or timeout
	for(int i = 0; i < 3; i++)
	{
		if(radio.write(&send, sizeof(Packet)))
		{
			status = 1;
			break;
		}
		
		shield.print("No response.\\nRetrying.");
	}
	
	if(status == 0) shield.print("Error: No radio\\nresponse.");
	
	else
	{
		radio.startListening();
		
		shield.print("Testing\\nconnection");
		
		//wait for success status after hub has connected.
		start = time(NULL);
		end = time(NULL);
		status = -1;
	
		
		
		while(difftime(end, start) < 30.0)
		{
			end = time(NULL);
			
			if(radio.available())
			{
				radio.read(&status, sizeof(int));
				break;
			}
			
			usleep(2);
		}
		
		radio.stopListening();
		
		if(status == 1) shield.print("Not connected");
		
		else if(status == -1) shield.print("Error: radio\\ntimeout.");
		
		else shield.print("Sensor is\\nconnected!");
		
	}
	
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
	
	int status = 0;
	
	if(signature == "nosig")
	{
		setSignature();
	}
	
	strcpy(send.signature, signature.c_str());
	
	shield.print("Sending \\nrequest");
	
	for(int i = 0; i < 3; i++)
	{
		if(radio.write(&send, sizeof(Packet)))
		{
			status = 1;
			break;
		}
		
		shield.print("No response.\\nRetrying.");
	}
	
	if(!status) shield.print("Error: No radio\\nresponse.");
	
	else
	{
		radio.startListening();
		
		shield.print("logging\\nmeasurement...");
		
		start = time(NULL);
		end = time(NULL);
		status = 1;
		
		while(difftime(end, start) < 30.0)
		{
			end = time(NULL);
			
			if(radio.available())
			{
				radio.read(&status, sizeof(int));
				break;
			}
		}
		
		if(status == 1) shield.print("Error: Radio timed out");
		else shield.print("Log successful");
		
		radio.stopListening();
	}
	
	sleep(2);
}

void PiSensor::upload()
{
	Packet send;
	int status = 0;
	time_t start;
	time_t end;
	
	send.flag = PacketFlag::UPLOAD;
	send.ambient = 0.0;
	send.object = 0.0;
	send.signature[0] = '\0';
	
	shield.print("sending request");
	
	for(int i = 0; i < 3; i++)
	{
		if(radio.write(&send, sizeof(Packet)))
		{
			status = 1;
			break;
		}
		
		shield.print("No response.\\nRetrying.");
	}
	
	if(status == 0) shield.print("Error: Radio\\ntimeout");
	
	else
	{
		radio.startListening();
		
		shield.print("Request sent.\\nWaiting...");
		status = 0;
		start = time(NULL);
		end = time(NULL);
		
		while(difftime(end, start) < 60.0)
		{
			end = time(NULL);
			
			if(radio.available())
			{
				radio.read(&status, sizeof(int));
				break;
			}
			usleep(2);
		}
		
		if(status == 1) shield.print("Could not\\nupload");
		else shield.print("Upload\\nsuccesful!");
		
		radio.stopListening();
	}
	
	sleep(2);
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
	Packet send;
	send.flag = PacketFlag::DISCONNECT;
	send.ambient = 0.0;
	send.object = 0.0;
	send.signature[0] = '\0';
	int status = 0;
	time_t start;
	time_t end;
	
	shield.print("sending request...");
	
	for(int i = 0; i < 3; i++)
	{
		if(radio.write(&send, sizeof(Packet)))
		{
			status = 1;
			break;
		}
		
		shield.print("No response.\\nRetrying.");
	}
	
	if(!status) shield.print("Error: Radio\\ntimeout");
	
	else
	{
		radio.startListening();
		
		shield.print("Request sent.\\nwaiting...");
		
		status = 0;
		start = time(NULL);
		end = time(NULL);
		
		while(difftime(end, start) < 30.0)
		{
			end = time(NULL);
			
			if(radio.available())
			{
				radio.read(&status, sizeof(int));
				break;
			}
		}
		
		if(status == 1) shield.print("Error\\ndisconnecting");
		else shield.print("Success!");
	}
	
	radio.stopListening();
	sleep(2);
}

void PiSensor::shutdown()
{
	shield.print("shutting down");
	sleep(2);
	shield.end();
	system("sudo shutdown -h now");
}

void PiSensor::update()
{
	int status = 0;
	time_t start;
	time_t end;
	Packet send;
	send.flag = PacketFlag::UPDATE;
	send.ambient = 0.0;
	send.object = 0.0;
	send.signature[0] = '\0';
	
	shield.print("sending request\\nfor update");
	for(int i = 0; i < 3; i++)
	{
		if(radio.write(&send, sizeof(Packet)))
		{
			status = 1;
			break;
		}
		
		shield.print("No response.\\nRetrying.");
	}
	
	if(!status) shield.print("Error: Radio\\ntimeout");
	
	else
	{
		radio.startListening();
		
		shield.print("Request sent.\\nwaiting...");
		
		status = 0;
		start = time(NULL);
		end = time(NULL);
		
		while(difftime(end, start) < 60.0)
		{
			end = time(NULL);
			
			if(radio.available())
			{
				radio.read(&status, sizeof(int));
				break;
			}
			usleep(2);
		}
		
		if(status == 0) shield.print("Pi Hub\\nupdated!");
		else shield.print("Error: Could\\nnot update");
		
		radio.stopListening();
	}
	
	sleep(2);
	
	shield.print("updating\\nPi Sensor");
	
	//now update the pi sensor
	status = system("ping github.com -c 1");
	status = WEXITSTATUS(status);
	std::cout << "exit status is " << status << std::endl;
	
	if(status != 0) 
	{
		shield.print("no wifi on\\nPi Sensor");
		sleep(2);
	}
	
	else
	{
		shield.print("updating. Sensor\\nwill restart");
		sleep(2);
		shield.end();
		system("python update.py PiSensor &");
		exit(0);
	}
}

void PiSensor::update_finish(int status)
{
	//exit 0 = updated
	//exit 1 = already up to date
	if(status == 0) shield.print("update\\nsuccesful!");
	else shield.print("Error Could\\nnot update");
	
	sleep(2);
}
