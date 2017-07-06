#include "../headers/PiHub.hpp"

PiHub::PiHub(uint16_t gpio, uint16_t ce) : radio(gpio, ce), pipes{"1Node", "2Node"}
{
	//setup member variables
	childRunning = false;
	
	//initialize the 3G shield. Make sure it's turned on
	system("stty -F /dev/ttyAMA0 115200");

	
	//setup the RF24 radio
	radio.begin();
	radio.setRetries(15, 15);
	radio.openWritingPipe(pipes[0]);
	radio.openReadingPipe(1, pipes[1]); 
	radio.setPALevel(RF24_PA_LOW);
	radio.setDataRate(RF24_250KBPS);
	//radio.setCRCLength(RF24_CRC_8);
	//radio.setPayloadSize(sizeof(Packet));
	//radio.write_register(DYNPD, 0);
	radio.startListening();
	radio.printDetails();
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
		usleep(10);
	
		//if the queue has packets in it, process them.
		if(!queue.isEmpty() || childRunning) processPacket();
	}
}

void PiHub::readRadio()
{
	buffer = new Packet;
	cout << "reading radio" << endl;
	//read packet into buffer
	radio.read(buffer, sizeof(Packet));
	usleep(10000);
	cout << "request is " << buffer->flag << endl;
	int success = 0;
	int firstSize = queue.getSize(), nextSize;
	
	//add to queue
	queue.enqueue(buffer);
	
	//send confirmation signal
	nextSize = queue.getSize();
	if(nextSize == firstSize + 1) 
	{
		success = 1;
		buffer = nullptr;
	}
	
	else
	{
		delete buffer;
	}
	
	cout << "returning " << success << endl;
	
	radio.stopListening();
	radio.write(&success, sizeof(int));
	sleep(1);
	radio.startListening();
	cout << "sent response" << endl;
}

void PiHub::processPacket()
{
	static pid_t pid;
	static pid_t end;
	static int status;
	static Packet *processed;
	
	//check if child is running
	if(childRunning)
	{
		//check if child ended
		end = waitpid(pid, &status, WNOHANG);
		cout << "end is " << end << endl;
		
		if(WIFEXITED(status))
		{
			cout << "child process ended" << endl;
			status = WEXITSTATUS(status);
			cout << "status is " << status << endl;
			childRunning = false;
			
			sendReturnMessage(status);
		}
	}
	
	//child not running
	else
	{
		//dequeue the next packet
		processed = queue.dequeue();
		
		//if the process asks to update, might as well update here.
		if(processed->flag == PacketFlag::UPDATE) update();
		
		childRunning = true;
		
		//fork the process
		pid = fork();
		
		//catch for child process
		if(pid == 0)
		{
			cout << "running child process" << endl;
			
			childProcess(processed);
		}
		
		cout << "pid is " << pid << endl;
		
		usleep(500000);
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
	string fileName, currentTime, buffer;
	vector<string> changes;
	bool good;
	
	//get the current time info
	time_t t = time(0);
	struct tm *timeInfo = localtime(&t);
	
	//format the file name for writing using string stream.
	ostringstream format;
	format << setfill('0') << setw(2) << (timeInfo->tm_mon + 1)
		<< "-" << setfill('0') << setw(2) << timeInfo->tm_mday << "-"
		<< (timeInfo->tm_year + 1900) << ".csv";
	fileName = format.str();
	
	//insert the current file into the changes vector
	changes.push_back(fileName);
	
	//find out whether or not the log file exists
	flog.open("./logs/" + fileName, fstream::in);
	good = flog.good();
	flog.close();
	
	//if the file doesn't exist, create it.
	if(!good)
	{
		flog.open(("./logs/" + fileName), fstream::out);
		flog << "time, signature, ambient temp, object temp" << endl;
	}
	
	//if the file does exist, open for appending
	else
	{
		flog.open("./logs/" + fileName, fstream::app);
	}
	
	//write out to the file
	flog << timeInfo->tm_hour << ":" << timeInfo->tm_min << ":" << timeInfo->tm_sec << ","
		<< processed->signature << "," << processed->ambient << "," << processed->object << endl;
	
	//close the file and delete the packet from memory
	delete processed;
	flog.close();
	
	//write out to pending changes file for the upload function
	flog.open("pendingChanges.txt", fstream::in);
	while(flog.good() && !flog.eof())
	{
		getline(flog, buffer);
		if(buffer != fileName && buffer != "") changes.push_back(buffer);
	}
	
	flog.close();
	flog.open("pendingChanges.txt", fstream::out);
	
	for(int i = 0; i < (int)changes.size(); i++)
	{
		flog << changes[i] << endl;
	}

	flog.close();
	
	exit(0);
}

void PiHub::upload()
{
	fstream pending("pendingChanges.txt", fstream::in);
	string buffer;
	int status = 0;
	
	while(!pending.eof())
	{
		//use flog to read in the log of files to upload
		getline(pending, buffer);
		
		if(buffer == "") break;
		
		cout << "loop" << endl;
		
		buffer = "./dropbox_uploader.sh upload ./logs/" + buffer +
			" " + buffer;
			
		status = system(buffer.c_str());	
		status = WEXITSTATUS(status);
		
		if(status != 0) break;
	}
	
	//wipe pending before continuing
	pending.close();
	pending.open("pendingChanges.txt", fstream::out);
	pending.close();
	//fstream::out automatically wipes the file
	
	exit(status);
	
}

void PiHub::test()
{
	int status = system("ping dropbox.com -c 1");
	cout << "exit status of ping is " << WEXITSTATUS(status);
	
	if(WEXITSTATUS(status) == 0) exit(0);
	
	else exit(1);
}

void PiHub::connect()
{
	cout << "child process connecting" << endl;
	//if(this->isConnected()) exit(1);
	
	int status = system("sudo ./sakis3g connect");
	cout << "status is " << WEXITSTATUS(status);
	cout << "system call done" << endl;
	
	if(WEXITSTATUS(status) == 0) exit(0);
	
	else exit(1);
}

void PiHub::disconnect()
{
	system("sudo ./sakis3g disconnect");
	cout << "finished disconnect call" << endl;
	exit(1);
}

void PiHub::sendReturnMessage(int status)
{
	usleep(1000);
	
	radio.stopListening();
	
	radio.write(&status, sizeof(int));
	
	cout << "sent return message " << status << endl;
	
	radio.startListening();
}

bool PiHub::isConnected()
{
	int status = system("ping dropbox.com -c 1");
	
	cout << "ping return value " << WEXITSTATUS(status) << endl;
	
	return WEXITSTATUS(status) == 0 ? true : false;
}

void PiHub::update()
{	
	system("python update.py PiHub &");
	exit(0);
}

void PiHub::update_finish(int status)
{
	//status 1 = process failed
	//status 0 = up-to-date
	radio.stopListening();
	
	radio.write(&status, sizeof(int));
	
	radio.startListening();
}
