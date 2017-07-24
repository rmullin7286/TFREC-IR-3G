#pragma once

#include "MLX90614.hpp"
#include "Adafruit_CharLCD_CPP.hpp"
#include <unistd.h>
#include <sstream>
#include <RF24/RF24.h>
#include <ctime>
#include <fstream>
#include <cstring>
#include <iostream>
#include <iomanip>

using std::ostringstream;
using std::fstream;

//a function for debugging and testing
void test();

//this is borrowed from the PiHub app for use here.
enum PacketFlag
{
	LOG, UPLOAD, TEST, CONNECT, DISCONNECT, UPDATE, AUTOLOG
};

//this is the standard configuration of the packet
//to be sent between the RF24 modules
struct Packet
{
	//A flag that tells the Pi Hub what type of packet it is.
	PacketFlag flag;

	//two floats for storing the ambient and object temperatures.
	float ambient;
	float object;

	//A signature that the user sets. This is inserted in the excel
	//file to let the user know what the measurement is.
	char signature[10];

	//operator overload for packet
	Packet & operator=(const Packet &rhs)
	{
		this->flag = rhs.flag;
		this->ambient = rhs.ambient;
		this->object = rhs.object;
		strcpy(this->signature, rhs.signature);

		return *this;
	}
};

// This is the main wrapper class for the whole program. This will be initialized and run in main,
// and all of the program will take place within this class.
class PiSensor
{
	// An enum to keep track of which menu option is being displayed or selected.
	enum MenuItem {CONNECT, TEST, LOG, AUTOLOG, UPLOAD, SET_SIG, DISCONNECT, SHUTDOWN, UPDATE, BACK};

public:
	//The GPIO pins for the rf24 will automatically be set to 25 and CE0 unless otherwise specified. This default
	//configuration should work for Raspberry Pi 3.
	PiSensor(uint16_t gpio=25, uint16_t ce=0);
	~PiSensor();

	//the main function that runs the whole app. This is initialized once in main
	//and everything else will be run inside the class.
	//This function will update the ambient and object temperatures to the screen every second unless the
	//user holds down the SELECT button. When the user does this they'll be taken to the menu.
	void run();

	//the function for catching when the program is initialized from the update.py script.
	void update_finish(int status);

private:

	//This is the function for the menu. The user can use the left and right buttons to scroll through
	//the different options and select them.
	void menu(double ambient, double object);

	//This is the main screen run from the run() function that prints the object and ambient to the screen.
	void mainScreen();

	//These functions are the ones that are run when the user selects the option from the menu.
	//They each carry out their respective commands.

	//Tell the hub to connect to 3G.
	void connect();

	//Test whether or not the hub is connected.
	void test();

	//Send the object and ambient temperature to the hub for logging.
	void log(double ambient, double object);

	//Logs the files automatically on a timer. Will be uploaded to dropbox once every 15 minutes
	void autoLogInit();

	//Tell the hub to upload the log files to dropbox.
	void upload();

	//Allows the user to set a signature so that later they can
	//tell what they've logged.
	void setSignature();

	//tell the hub to disconnect from 3G
	void disconnect();

	//Shuts down the system
	void shutdown();

	//Runs the update script to update the program to the latest version.
	void update();

	//go back to the mainScreen() function.
	void back();

	MLX90614 sensor;
	LCDShield shield;
	RF24 radio;
	string signature;
	fstream save;
	bool isAutoLogging;
	double interval;
	time_t autoLogStart;
	time_t autoLogDuration;

	const uint8_t pipes[2][6];
};
