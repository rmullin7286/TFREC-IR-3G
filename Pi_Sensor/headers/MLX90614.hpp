#pragma once
#include <bcm2835.h>
#include <iostream>
#include <unistd.h>

/******************************************************************************
 * AUTHOR: RYAN MULLIN   https://github.com/rmullin7286
 * 
 * This is a simple wrapper class for the mlx90614. This uses the
 * airspayce bcm2835 library for i2c connection. This class was designed
 * for communication between the Raspberry Pi and the mlx90614 through
 * i2c connection. 
 * 
 * link for bcm2835 library: http://www.airspayce.com/mikem/bcm2835/index.html
 * ****************************************************************************/

class MLX90614
{
public:
	
	//enums for which temp scale (C, F, or K)
	enum {CELSIUS, FARENHEIT, KELVIN};
	
	//some static consts defined for addresses
	static const unsigned int address = 0x5a;
	static const unsigned int ambient = 0x06;
	static const unsigned int object = 0x07;
	
	MLX90614()
	{
		bcm2835_init();
		bcm2835_i2c_set_baudrate(38400);
		scale = CELSIUS;
	}
	
	~MLX90614()
	{
		
	}
	
	void setScale(int temp)
	{
		scale = temp;
	}
	
	double readAmbient()
	{
		char measurement = ambient;
		
		bcm2835_i2c_begin();
		bcm2835_i2c_setSlaveAddress(address);
		bcm2835_i2c_write(&measurement, 1);
		bcm2835_i2c_read_register_rs(&measurement, &mlxBuf[0], 3);

		
		tempBuf = (double) (((mlxBuf[1]) << 8) + mlxBuf[0]);
		tempBuf = (tempBuf * 0.02) - 0.01;
		
		if(scale == CELSIUS) tempBuf -= 273.15;
		
		else if(scale == FARENHEIT) tempBuf = (tempBuf * 9/5) - 459.67;
		
		return tempBuf;

	}
	
	double readObject()
	{
		char measurement = object;
		
		bcm2835_i2c_begin();
		bcm2835_i2c_setSlaveAddress(address);
		bcm2835_i2c_write(&measurement, 1);
		bcm2835_i2c_read_register_rs(&measurement, &mlxBuf[0], 3);
		
		tempBuf = (double) (((mlxBuf[1]) << 8) + mlxBuf[0]);
		tempBuf = (tempBuf * 0.02) - 0.01;
		
		if(scale == CELSIUS) tempBuf -= 273.15;
		
		else if(scale == FARENHEIT) tempBuf = (tempBuf * 9/5) - 459.67;
		
		return tempBuf;
	}
		
private:
	
	double tempBuf;
	char mlxBuf[6];
	int scale;
};
