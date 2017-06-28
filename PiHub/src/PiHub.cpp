#include "../headers/PiHub.hpp"

PiHub::PiHub(uint16_t gpio, uint16_t ce) : radio(gpio, ce), pipes{"1Node", "2Node"}
{
	//setup member variables
	buffer = new Packet;
	childRunning = false;
	
	//initialize the 3G shield. Make sure it's turned on
	//system("stty -F /dev/ttyAMA0 115200");
	//system("sudo ./sakis3g connect");
	
	//setup the RF24 radio
	radio.begin();
	radio.setRetries(15, 15);
	radio.openWritingPipe(pipes[0]);
	radio.openReadingPipe(1, pipes[1]); 
	radio.setPALevel(RF24_PA_MAX);
	radio.startListening();
}

PiHub::~PiHub()
{
	delete buffer;
}

void PiHub::run()
{
	while(1)
	{
		//if the radio is available, read the packet into
		//the queue.
		if(radio.available()) readRadio();
	
		//if the queue has packets in it, process them.
		if(!queue.isEmpty() || childRunning) processPacket();
	}
}

void PiHub::readRadio()
{
	//read packet into buffer
	radio.read(buffer, sizeof(Packet));
	
	bool success = false;
	int firstSize = queue.getSize(), nextSize;
	
	//add to queue
	queue.enqueue(buffer);
	
	//send confirmation signal
	nextSize = queue.getSize();
	if(nextSize == firstSize + 1) success = true;
	
	radio.stopListening();
	radio.write(&success, sizeof(bool));
	radio.startListening();
}

void PiHub::processPacket()
{
	static pid_t pid;
	static int status;
	static Packet *processed;
	
	//check if child is running
	if(childRunning)
	{
		//check if child ended
		pid = waitpid(-1, &status, WNOHANG);
		
		if(WIFEXITED(status))
		{
			status = WEXITSTATUS(status);
			childRunning = false;
			
			sendReturnMessage(status);
		}
	}
	
	//child not running
	else
	{
		//dequeue the next packet
		processed = queue.dequeue();
		childRunning = true;
		
		//fork the process
		pid = fork();
		
		//catch for child process
		if(pid == 0)
		{
			childProcess(processed);
		}
	}
}

void PiHub::childProcess(Packet *processed)
{
	switch(processed->flag)
	{
	case LOG: log(processed);
		break;
	case UPLOAD: delete processed; 
		upload();
		break;
	case TEST: delete processed; 
		test();
		break;
	case CONNECT: delete processed;
		connect();
		break;
	case DISCONNECT: delete processed; 
		disconnect();
		break;
	}
}

void PiHub::log(Packet *processed)
{
	string fileName, currentTime;
	
	//get the current time info
	time_t t = time(0);
	struct tm *timeInfo = localtime(&t);
	
	//format the file name for writing using string stream.
	ostringstream format;
	format << setfill('0') << setw(2) << timeInfo->tm_mday
		<< "-" << setfill('0') << setw(2) << (timeInfo->tm_mon + 1) << "-"
		<< (timeInfo->tm_year + 1900) << ".csv";
	fileName = format.str();
	
	//if the file doesn't exist, create it.
	if(access(fileName.c_str(), F_OK) != -1)
	{
		flog.open(("./logs/" + fileName), fstream::out);
		flog << "time, signature, ambient temp, object temp" << endl;
	}
	
	//if the file does exist, open for appending
	else
	{
		flog.open(fileName, fstream::app);
	}
	
	//write out to the file
	flog << timeInfo->tm_hour << ":" << timeInfo->tm_min << ":" << timeInfo->tm_sec << ","
		<< processed->signature << "," << processed->ambient << "," << processed->object << endl;
	
	//close and exit the program
	delete processed;
	flog.close();
	
	//write out to pending changes file for the upload function
	flog.open("pendingChanges.txt", fstream::app);
	flog << fileName << endl;
	flog.close();
	
	exit(EXIT_LOGSUCCESS);
}

void PiHub::upload()
{
	fstream pending("pendingChanges.txt", fstream::app);
	string buffer;
	int status;
	
	while(!pending.eof())
	{
		//use flog to read in the log of files to upload
		getline(pending, buffer);
		
		buffer = "./dropbox_uploader.sh upload ./logs/" + buffer +
			" " + buffer;
			
		status = system(buffer.c_str());
		
		if(WEXITSTATUS(status) == 1) exit(EXIT_UPLOADFAIL);  
	}
	
	//wipe pending before continuing
	pending.close();
	pending.open("pendingChanges.txt", fstream::out);
	
}

void PiHub::test()
{
	int status = system("ping dropbox.com -c 1");
	
	if(WEXITSTATUS(status) == 0) exit(EXIT_TESTSUCCESS);
	
	else exit(EXIT_NOCONNECTION);
}

void PiHub::connect()
{
	if(this->isConnected()) exit(1);
	
	system("stty -F /dev/ttyAMA0 115200");
	
	int status = system("sudo ./sakis3g connect");
	
	if(WEXITSTATUS(status) == 0) exit(1);
	
	else exit(-1);
}

void PiHub::disconnect()
{
	system("sudo ./sakis3g disconnect");
	exit(EXIT_DISCONNECT);
}

void PiHub::sendReturnMessage(int status)
{
	usleep(10);
	
	radio.stopListening();
	
	radio.write(&status, sizeof(int));
	
	radio.startListening();
}

bool PiHub::isConnected()
{
	int status = system("ping dropbox.com -c 1");
	
	return WEXITSTATUS(status) == 0 ? true : false;
}
