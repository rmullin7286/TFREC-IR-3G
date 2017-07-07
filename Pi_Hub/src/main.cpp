#include "../headers/PiHub.hpp"

int main(int argc, char *argv[])
{
	//test();
	
	PiHub app;
	
	//for catching startup after update
	if(argc > 1)
	{
		cout << "running update finish" << endl;
		int status = atoi(argv[1]);
		app.update_finish(status);
	}
	
	app.run();
	return 0;
}


void test()
{
	Packet *test = new Packet;
	test->ambient = 10.0;
	test->object = 20.0;
	strcpy(test->signature, "test");
	test->flag = PacketFlag::UPLOAD;
	PiHub app;
	app.log(test);
}
