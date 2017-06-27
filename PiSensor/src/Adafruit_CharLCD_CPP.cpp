#include "../headers/Adafruit_CharLCD_CPP.hpp"

LCDShield::LCDShield()
{
	//I chose to get all the initialization done here to reduce the overhead
	//of recalling everything
	//if(Py_IsInitialized()) Py_Finalize();
	
	//Py_Initialize();
	
	//pModule = PyImport_AddModule("__main__"); //create main module
	
	//PyRun_SimpleString(
	//"import sys\n"
	//"import Adafruit_CharLCD as LCD\n"
	//"old_stdout = sys.stdout\n"
	//"class CatchStdOut:\n"
	//"	def __init__(self):\n"
	//"		self.value = ''\n"
	//"	def write(self, txt):\n"
	//"		self.value = txt\n"
	//"sys.stdout = catchStdOut\n"
	//"lcd = LCD.Adafruit_CharLCDPlate()\n"
	//"buttons = ( LCD.SELECT, LCD.LEFT, LCD.UP, LCD.DOWN, LCD.RIGHT)\n");
}

bool LCDShield::init()
{	
	
	if(Py_IsInitialized()) return false;
	
	Py_Initialize();
	
	pMain = PyImport_AddModule("__main__"); //create main module
	PyRun_SimpleString(
	"import Adafruit_CharLCD as LCD\n"
	"lcd = LCD.Adafruit_CharLCDPlate()\n"
	"buttons = ( LCD.SELECT, LCD.LEFT, LCD.UP, LCD.DOWN, LCD.RIGHT )\n"); 
	
	PyErr_Print();
	
	return true;
}

bool LCDShield::init_compatible()
{	
	system("gpio -x mcp23017:100:0x20 mode 106 out");
	
	if(Py_IsInitialized()) return false;
	
	Py_Initialize();
	
	pMain = PyImport_AddModule("__main__"); //create main module
	PyRun_SimpleString(
	"import Adafruit_CharLCD as LCD\n"
	"lcd = LCD.Adafruit_CharLCDPlate()\n"
	"buttons = ( LCD.SELECT, LCD.LEFT, LCD.UP, LCD.DOWN, LCD.RIGHT )\n"); 
	
	PyErr_Print();
	
	return true;
}

bool LCDShield::end()
{
	system("gpio -x mcp23017:100:0x20 mode 106 out");
	
	if(Py_IsInitialized())
	{
	    PyRun_SimpleString(
	    "lcd.clear()\n"
	    "lcd.set_color(0.0, 0.0, 0.0)\n");
		
		Py_Finalize();
		
		return true;
	}
	
	return false;
}

LCDShield::~LCDShield()
{
	//this code is just to make sure that the screen is cleared and
	//turned off before exiting the program
			
	//PyRun_SimpleString(
	//"lcd.clear()\n"
	//"lcd.set_color(0.0,0.0,0.0)\n"
	//"sys.stdout = old_stdout");
		
	//Py_Finalize();
}

void LCDShield::print(const string &input)
{
	system("gpio -x mcp23017:100:0x20 mode 106 out");
	
	string command =
    "lcd.clear()\n"
	"lcd.message('" + input + "')\n";
	
	PyRun_SimpleString(command.c_str());
}

Button LCDShield::getButton()
{
	system("gpio -x mcp23017:100:0x20 mode 106 out");
	
	//NOBUTTON = 0
	//SELECT = 1
	//LEFT = 2
	//UP = 3
	//DOWN = 4
	//RIGHT = 5
	//
	Button button;
	
	PyRun_SimpleString(
	"count = 0\n"
	"cButton = 0\n"
	"for button in buttons:\n"
	"	count += 1\n"
	"	if lcd.is_pressed(button):\n"
	"		cButton = count\n");
	
	value = PyObject_GetAttrString(pMain, "cButton");
	
	int valueAsInt = PyInt_AsLong(value);
	
	button = static_cast<Button>(valueAsInt);
	
	return button;
}

void LCDShield::isBacklit(bool backlit)
{
	system("gpio -x mcp23017:100:0x20 mode 106 out");
	backlit ? PyRun_SimpleString("lcd.set_color(1.0, 0.0, 0.0)\n") : PyRun_SimpleString("lcd.set_color(0.0, 0.0, 0.0)\n");
}
