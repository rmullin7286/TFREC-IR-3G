#ifndef PIHUB_HPP
#define PIHUB_HPP

#include <RF24/RF24.h>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctime>
#include <string>
#include <sstream>
#include <iomanip>
#include "PacketQueue.hpp"
#include <unistd.h>

using std::fstream;
using std::string;
using std::ostringstream;
using std::setfill;
using std::setw;
using std::endl;
using std::cout;

enum exitStatus{EXIT_LOGSUCCESS, EXIT_UPLOADSUCCESS, EXIT_UPLOADFAIL, EXIT_TESTSUCCESS, EXIT_NOCONNECTION, EXIT_CONNECTED, EXIT_DISCONNECT};

//wrapper class for app
class PiHub
{	
public:
	PiHub(uint16_t gpio=25, uint16_t ce=0);
	~PiHub();
	
	void run();
	

	
private:
	void readRadio();
	void processPacket();
	void childProcess(Packet *processed);
	
	void log(Packet *processed);
	void upload();
	void test();
	void connect();
	void disconnect();
	
	void sendReturnMessage(int status);
	
	bool isConnected();
		
	fstream flog;
	fstream dataFile;
	
	PacketQueue queue;
	Packet *buffer;
	RF24 radio;
	const uint8_t pipes[2][6]; // more pipes can be added later if we need them
	
	bool childRunning;
	
};

//just a function for testing
void test();


#endif
