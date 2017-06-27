#include "../headers/PiHub.hpp"

int main(void)
{
	//test();
	
	PiHub app;
	return 0;
}


void test()
{
	bool empty;
	
	PacketQueue queue;
	empty = queue.isEmpty();
	
	Packet *test = new Packet;
	test->flag = UPLOAD;
	test->object = 10.10;
	test->ambient = 20.20;
	strcpy(test->signature, "APPLE");
	
	queue.enqueue(test);
	
	empty = queue.isEmpty();
	
	Packet *test2 = new Packet;
	
	test2 = queue.dequeue();
	
	test = queue.dequeue();
}
