#pragma once

#include "MLX90614.hpp"
#include "Adafruit_CharLCD_CPP.hpp"
#include <unistd.h>
#include <sstream>
#include <RF24/RF24.h>
#include <ctime>
#include <fstream>
#include <cstring>

using std::ostringstream;
using std::fstream;

void test();

//this is borrowed from the PiHub app for use here.
enum PacketFlag
{
	LOG, UPLOAD, TEST, CONNECT, DISCONNECT, UPDATE
};

enum exitStatus{EXIT_LOGSUCCESS, EXIT_UPLOADSUCCESS, EXIT_UPLOADFAIL, EXIT_TESTSUCCESS, EXIT_NOCONNECTION, EXIT_CONNECTED, EXIT_DISCONNECT};

//this is the standard configuration of the packet
//to be sent between the RF24 modules
struct Packet
{
	PacketFlag flag;
	float ambient;
	float object;
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


class PiSensor
{
	enum MenuItem {CONNECT, TEST, LOG, UPLOAD, SET_SIG, DISCONNECT, SHUTDOWN, UPDATE, BACK};
	
public:
	PiSensor(uint16_t gpio=25, uint16_t ce=0);
	~PiSensor();
	
	void run();
	void update_finish(int status);
	
private:
	void menu(double ambient, double object);
	void mainScreen();
	
	void connect();
	void test();
	void log(double ambient, double object);
	void upload();
	void setSignature();
	void disconnect();
	void shutdown();
	void update();
	void back();
	
	MLX90614 sensor;
	LCDShield shield;
	RF24 radio;
	string signature;
	fstream save;
	
	const uint8_t pipes[2][6];
};
