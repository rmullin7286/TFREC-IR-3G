#include "../headers/PiSensor.hpp"

int main(int argc, char *argv[])
{
	PiSensor app;
	if(argc > 1)
	{
		int status = atoi(argv[1]);
		app.update_finish(status);
	}
	app.run();
}
