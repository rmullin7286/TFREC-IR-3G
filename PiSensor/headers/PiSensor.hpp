#pragma once

#include "MLX90614.hpp"
#include "Adafruit_CharLCD_CPP.hpp"
#include <unistd.h>
#include <sstream>

using std::ostringstream;

//this is borrowed from the PiHub app for use here.
enum PacketFlag
{
	LOG, UPLOAD, TEST, CONNECT, DISCONNECT
};

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
	enum MenuItem {CONNECT, TEST, LOG, UPLOAD, SET_SIG, DISCONNECT, SHUTDOWN, BACK};
	
public:
	PiSensor();
	~PiSensor();
	
	void run();
	
private:
	void menu();
	void mainScreen();
	
	void connect();
	void test();
	void log();
	void upload();
	void setSignature();
	void disconnect();
	void shutdown();
	void back();
	
	MLX90614 sensor;
	LCDShield shield;
};
