#include "../headers/PiSensor.hpp"

int main(int argc, char *argv[])
{
	//This is a small test function in case I need to text anything.
	//test();
	
	//Initialize the app
	PiSensor app;
	
	//check and see if the program has been started from the update script.
	//If so, notify the user.
	if(argc > 1)
	{
		int status = atoi(argv[1]);
		app.update_finish(status);
	}
	
	//run the app
	app.run();
}

void test()
{
	std::cout << sizeof(Packet);
}
