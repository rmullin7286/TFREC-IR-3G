#include "../headers/PiHub.hpp"

int main(int argc, char *argv[])
{
	test();
	
	PiHub app;
	
	//for catching startup after update
	if(argc > 1)
	{
		int status = atoi(argv[1]);
		app.update_finish(status);
	}
	
	app.run();
	return 0;
}


void test()
{
	int x = static_cast<int>(PacketFlag::UPDATE);
	int y = static_cast<int>(PacketFlag::CONNECT);
	usleep(3);
}
