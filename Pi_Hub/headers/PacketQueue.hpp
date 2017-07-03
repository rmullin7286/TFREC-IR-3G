#ifndef PACKETQUEUE_HPP
#define PACKETQUEUE_HPP

#include <cstring>

//These are the different flags that tell the program
//what to do:
//LOG: Log the packet info to the csv file
//UPLOAD: upload current csv info to DropBox
//TEST: test the connection to make sure everything's working
//CONNECT: if not connected, connect the PiHub to 3G
//DISCONNECT: disconnect PiHub
enum PacketFlag
{
	LOG, UPLOAD, TEST, CONNECT, DISCONNECT, UPDATE
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





class PacketQueue
{
	//this is the node that will contain the packet, along
	//with a pointer to the next packet.
	struct PacketNode
	{
		Packet data;
		PacketNode *next;
	};
	
public:
	PacketQueue() 
	{
		front = nullptr;
		back = nullptr;
		size = 0;
	}
	
	~PacketQueue() 
	{
		if(!isEmpty())
		{
			back = front;
			
			do
			{
				back = back->next;
				delete front;
				front = back;
			}while(front != nullptr);
		}
	}
	
	//for inserting a new packet into the queue
	void enqueue(const Packet *newPacket)
	{	
		PacketNode *newNode = new PacketNode;
		newNode->data = *newPacket;
		
		if(this->isEmpty())
		{
			this->front = newNode;
			this->back = newNode;
		}
		
		else
		{
			this->back->next = newNode;
			newNode->next = nullptr;
			this->back = newNode;
		}
		
		size++;
	}
	
	//for removing packet from queue
	Packet *dequeue()
	{
		Packet *remove = new Packet;
		
		if(size == 1)
		{
			*remove = front->data;
			delete front;
			front = nullptr;
			back = nullptr;
			
			size--;
		}
		
		else if(!isEmpty())
		{
			PacketNode *temp = front;
			front = front->next;
			*remove = temp->data;
			delete temp;
			
			size--;
		}
		
		return remove;
	}
	
	//for checking if queue is empty
	bool isEmpty() const
	{
		if (size == 0)
		{
			return true;
		}
		
		return false;
	}
	
	//for getting size
	int getSize() const
	{
		return size;
	}
	
private:
	
	int size;
	PacketNode *front;
	PacketNode *back;
};

#endif
