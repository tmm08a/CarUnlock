/*
Tim Michaud

tmm08a@acu.edu

PhidgetCarUnlock
*/

#include <stdio.h>
#include <phidget21.h>
#include <windows.h>
#include <iostream>
using namespace std;

/*
Explanation and number sequence here: http://everything2.com/title/Weak+security+in+our+daily+lives
TL;DR of link:

The sequence array is the sequence of numbers entered by the machineit is in the order so no numbers are repeated,
it is the fastest way to unlock the car door (by dictionary attack)

The numbers 2/4/6/8 aren't used because they aren't needed. (Each number coupling only has one actual button to press, they are not
unique)

There are two breakdowns: 1/2 3/4 5/6 7/8 9/0 And 0/1 2/3 4/5 6/7 8/9

The even numbers(including 0) are not used. Odds are because it will cover both cases of keypads starting with 0 or 1, 
and also to keep it simple


A little experimentation will reveal that, if the code is 11357, and you type 5113579, the door will still open!
This means that with 7 characters we managed to try out 3 sequences - 51135, 11357, and 13579. 

After the inital 4 numbers (which sort of primed the pump) every digit tries one new sequence. 
Since there are 55 length 5 sequences of characters from an alphabet of size 5, we know that we'll need to try 3125 
sequences total. With our intuition from above, we would hope that we could find a sequence of size 4 + 3125 
(priming the pump, followed by one new sequence every keypress). 

It turns out that a mathematician named de Bruijn hasalready done all of the hard work for us on this one, and all 
of the relevant math can found under the names de Bruijn sequence and de Bruijn graph. But I'm not going to talk about
math any further here. Right now, I am going to give you a sequence of minimal length that, when you enter it into a 
car's numeric keypad, is guaranteed to unlock the doors of said car. It is exactly 3129 keypresses long. 

*/
char sequence[] ={
  '9','9','9','9','1','1','1','1','1','3','1','1','1','1','5','1','1','1','1','7','1','1','1','1','9','1','1','1','3','3','1',
  '1','1','3','5','1','1','1','3','7','1','1','1','3','9','1','1','1','5','3','1','1','1','5','5','1','1','1','5','7','1','1',
  '1','5','9','1','1','1','7','3','1','1','1','7','5','1','1','1','7','7','1','1','1','7','9','1','1','1','9','3','1','1','1',
  '9','5','1','1','1','9','7','1','1','1','9','9','1','1','3','1','3','1','1','3','1','5','1','1','3','1','7','1','1','3','1',
  '9','1','1','3','3','3','1','1','3','3','5','1','1','3','3','7','1','1','3','3','9','1','1','3','5','3','1','1','3','5','5',
  '1','1','3','5','7','1','1','3','5','9','1','1','3','7','3','1','1','3','7','5','1','1','3','7','7','1','1','3','7','9','1',
  '1','3','9','3','1','1','3','9','5','1','1','3','9','7','1','1','3','9','9','1','1','5','1','3','1','1','5','1','5','1','1',
  '5','1','7','1','1','5','1','9','1','1','5','3','3','1','1','5','3','5','1','1','5','3','7','1','1','5','3','9','1','1','5',
  '5','3','1','1','5','5','5','1','1','5','5','7','1','1','5','5','9','1','1','5','7','3','1','1','5','7','5','1','1','5','7',
  '7','1','1','5','7','9','1','1','5','9','3','1','1','5','9','5','1','1','5','9','7','1','1','5','9','9','1','1','7','1','3',
  '1','1','7','1','5','1','1','7','1','7','1','1','7','1','9','1','1','7','3','3','1','1','7','3','5','1','1','7','3','7','1',
  '1','7','3','9','1','1','7','5','3','1','1','7','5','5','1','1','7','5','7','1','1','7','5','9','1','1','7','7','3','1','1',
  '7','7','5','1','1','7','7','7','1','1','7','7','9','1','1','7','9','3','1','1','7','9','5','1','1','7','9','7','1','1','7',
  '9','9','1','1','9','1','3','1','1','9','1','5','1','1','9','1','7','1','1','9','1','9','1','1','9','3','3','1','1','9','3',
  '5','1','1','9','3','7','1','1','9','3','9','1','1','9','5','3','1','1','9','5','5','1','1','9','5','7','1','1','9','5','9',
  '1','1','9','7','3','1','1','9','7','5','1','1','9','7','7','1','1','9','7','9','1','1','9','9','3','1','1','9','9','5','1',
  '1','9','9','7','1','1','9','9','9','1','3','1','3','3','1','3','1','3','5','1','3','1','3','7','1','3','1','3','9','1','3',
  '1','5','3','1','3','1','5','5','1','3','1','5','7','1','3','1','5','9','1','3','1','7','3','1','3','1','7','5','1','3','1',
  '7','7','1','3','1','7','9','1','3','1','9','3','1','3','1','9','5','1','3','1','9','7','1','3','1','9','9','1','3','3','1',
  '5','1','3','3','1','7','1','3','3','1','9','1','3','3','3','3','1','3','3','3','5','1','3','3','3','7','1','3','3','3','9',
  '1','3','3','5','3','1','3','3','5','5','1','3','3','5','7','1','3','3','5','9','1','3','3','7','3','1','3','3','7','5','1',
  '3','3','7','7','1','3','3','7','9','1','3','3','9','3','1','3','3','9','5','1','3','3','9','7','1','3','3','9','9','1','3',
  '5','1','5','1','3','5','1','7','1','3','5','1','9','1','3','5','3','3','1','3','5','3','5','1','3','5','3','7','1','3','5',
  '3','9','1','3','5','5','3','1','3','5','5','5','1','3','5','5','7','1','3','5','5','9','1','3','5','7','3','1','3','5','7',
  '5','1','3','5','7','7','1','3','5','7','9','1','3','5','9','3','1','3','5','9','5','1','3','5','9','7','1','3','5','9','9',
  '1','3','7','1','5','1','3','7','1','7','1','3','7','1','9','1','3','7','3','3','1','3','7','3','5','1','3','7','3','7','1',
  '3','7','3','9','1','3','7','5','3','1','3','7','5','5','1','3','7','5','7','1','3','7','5','9','1','3','7','7','3','1','3',
  '7','7','5','1','3','7','7','7','1','3','7','7','9','1','3','7','9','3','1','3','7','9','5','1','3','7','9','7','1','3','7',
  '9','9','1','3','9','1','5','1','3','9','1','7','1','3','9','1','9','1','3','9','3','3','1','3','9','3','5','1','3','9','3',
  '7','1','3','9','3','9','1','3','9','5','3','1','3','9','5','5','1','3','9','5','7','1','3','9','5','9','1','3','9','7','3',
  '1','3','9','7','5','1','3','9','7','7','1','3','9','7','9','1','3','9','9','3','1','3','9','9','5','1','3','9','9','7','1',
  '3','9','9','9','1','5','1','5','3','1','5','1','5','5','1','5','1','5','7','1','5','1','5','9','1','5','1','7','3','1','5',
  '1','7','5','1','5','1','7','7','1','5','1','7','9','1','5','1','9','3','1','5','1','9','5','1','5','1','9','7','1','5','1',
  '9','9','1','5','3','1','7','1','5','3','1','9','1','5','3','3','3','1','5','3','3','5','1','5','3','3','7','1','5','3','3',
  '9','1','5','3','5','3','1','5','3','5','5','1','5','3','5','7','1','5','3','5','9','1','5','3','7','3','1','5','3','7','5',
  '1','5','3','7','7','1','5','3','7','9','1','5','3','9','3','1','5','3','9','5','1','5','3','9','7','1','5','3','9','9','1',
  '5','5','1','7','1','5','5','1','9','1','5','5','3','3','1','5','5','3','5','1','5','5','3','7','1','5','5','3','9','1','5',
  '5','5','3','1','5','5','5','5','1','5','5','5','7','1','5','5','5','9','1','5','5','7','3','1','5','5','7','5','1','5','5',
  '7','7','1','5','5','7','9','1','5','5','9','3','1','5','5','9','5','1','5','5','9','7','1','5','5','9','9','1','5','7','1',
  '7','1','5','7','1','9','1','5','7','3','3','1','5','7','3','5','1','5','7','3','7','1','5','7','3','9','1','5','7','5','3',
  '1','5','7','5','5','1','5','7','5','7','1','5','7','5','9','1','5','7','7','3','1','5','7','7','5','1','5','7','7','7','1',
  '5','7','7','9','1','5','7','9','3','1','5','7','9','5','1','5','7','9','7','1','5','7','9','9','1','5','9','1','7','1','5',
  '9','1','9','1','5','9','3','3','1','5','9','3','5','1','5','9','3','7','1','5','9','3','9','1','5','9','5','3','1','5','9',
  '5','5','1','5','9','5','7','1','5','9','5','9','1','5','9','7','3','1','5','9','7','5','1','5','9','7','7','1','5','9','7',
  '9','1','5','9','9','3','1','5','9','9','5','1','5','9','9','7','1','5','9','9','9','1','7','1','7','3','1','7','1','7','5',
  '1','7','1','7','7','1','7','1','7','9','1','7','1','9','3','1','7','1','9','5','1','7','1','9','7','1','7','1','9','9','1',
  '7','3','1','9','1','7','3','3','3','1','7','3','3','5','1','7','3','3','7','1','7','3','3','9','1','7','3','5','3','1','7',
  '3','5','5','1','7','3','5','7','1','7','3','5','9','1','7','3','7','3','1','7','3','7','5','1','7','3','7','7','1','7','3',
  '7','9','1','7','3','9','3','1','7','3','9','5','1','7','3','9','7','1','7','3','9','9','1','7','5','1','9','1','7','5','3',
  '3','1','7','5','3','5','1','7','5','3','7','1','7','5','3','9','1','7','5','5','3','1','7','5','5','5','1','7','5','5','7',
  '1','7','5','5','9','1','7','5','7','3','1','7','5','7','5','1','7','5','7','7','1','7','5','7','9','1','7','5','9','3','1',
  '7','5','9','5','1','7','5','9','7','1','7','5','9','9','1','7','7','1','9','1','7','7','3','3','1','7','7','3','5','1','7',
  '7','3','7','1','7','7','3','9','1','7','7','5','3','1','7','7','5','5','1','7','7','5','7','1','7','7','5','9','1','7','7',
  '7','3','1','7','7','7','5','1','7','7','7','7','1','7','7','7','9','1','7','7','9','3','1','7','7','9','5','1','7','7','9',
  '7','1','7','7','9','9','1','7','9','1','9','1','7','9','3','3','1','7','9','3','5','1','7','9','3','7','1','7','9','3','9',
  '1','7','9','5','3','1','7','9','5','5','1','7','9','5','7','1','7','9','5','9','1','7','9','7','3','1','7','9','7','5','1',
  '7','9','7','7','1','7','9','7','9','1','7','9','9','3','1','7','9','9','5','1','7','9','9','7','1','7','9','9','9','1','9',
  '1','9','3','1','9','1','9','5','1','9','1','9','7','1','9','1','9','9','1','9','3','3','3','1','9','3','3','5','1','9','3',
  '3','7','1','9','3','3','9','1','9','3','5','3','1','9','3','5','5','1','9','3','5','7','1','9','3','5','9','1','9','3','7',
  '3','1','9','3','7','5','1','9','3','7','7','1','9','3','7','9','1','9','3','9','3','1','9','3','9','5','1','9','3','9','7',
  '1','9','3','9','9','1','9','5','3','3','1','9','5','3','5','1','9','5','3','7','1','9','5','3','9','1','9','5','5','3','1',
  '9','5','5','5','1','9','5','5','7','1','9','5','5','9','1','9','5','7','3','1','9','5','7','5','1','9','5','7','7','1','9',
  '5','7','9','1','9','5','9','3','1','9','5','9','5','1','9','5','9','7','1','9','5','9','9','1','9','7','3','3','1','9','7',
  '3','5','1','9','7','3','7','1','9','7','3','9','1','9','7','5','3','1','9','7','5','5','1','9','7','5','7','1','9','7','5',
  '9','1','9','7','7','3','1','9','7','7','5','1','9','7','7','7','1','9','7','7','9','1','9','7','9','3','1','9','7','9','5',
  '1','9','7','9','7','1','9','7','9','9','1','9','9','3','3','1','9','9','3','5','1','9','9','3','7','1','9','9','3','9','1',
  '9','9','5','3','1','9','9','5','5','1','9','9','5','7','1','9','9','5','9','1','9','9','7','3','1','9','9','7','5','1','9',
  '9','7','7','1','9','9','7','9','1','9','9','9','3','1','9','9','9','5','1','9','9','9','7','1','9','9','9','9','3','3','3',
  '3','3','5','3','3','3','3','7','3','3','3','3','9','3','3','3','5','5','3','3','3','5','7','3','3','3','5','9','3','3','3',
  '7','5','3','3','3','7','7','3','3','3','7','9','3','3','3','9','5','3','3','3','9','7','3','3','3','9','9','3','3','5','3',
  '5','3','3','5','3','7','3','3','5','3','9','3','3','5','5','5','3','3','5','5','7','3','3','5','5','9','3','3','5','7','5',
  '3','3','5','7','7','3','3','5','7','9','3','3','5','9','5','3','3','5','9','7','3','3','5','9','9','3','3','7','3','5','3',
  '3','7','3','7','3','3','7','3','9','3','3','7','5','5','3','3','7','5','7','3','3','7','5','9','3','3','7','7','5','3','3',
  '7','7','7','3','3','7','7','9','3','3','7','9','5','3','3','7','9','7','3','3','7','9','9','3','3','9','3','5','3','3','9',
  '3','7','3','3','9','3','9','3','3','9','5','5','3','3','9','5','7','3','3','9','5','9','3','3','9','7','5','3','3','9','7',
  '7','3','3','9','7','9','3','3','9','9','5','3','3','9','9','7','3','3','9','9','9','3','5','3','5','5','3','5','3','5','7',
  '3','5','3','5','9','3','5','3','7','5','3','5','3','7','7','3','5','3','7','9','3','5','3','9','5','3','5','3','9','7','3',
  '5','3','9','9','3','5','5','3','7','3','5','5','3','9','3','5','5','5','5','3','5','5','5','7','3','5','5','5','9','3','5',
  '5','7','5','3','5','5','7','7','3','5','5','7','9','3','5','5','9','5','3','5','5','9','7','3','5','5','9','9','3','5','7',
  '3','7','3','5','7','3','9','3','5','7','5','5','3','5','7','5','7','3','5','7','5','9','3','5','7','7','5','3','5','7','7',
  '7','3','5','7','7','9','3','5','7','9','5','3','5','7','9','7','3','5','7','9','9','3','5','9','3','7','3','5','9','3','9',
  '3','5','9','5','5','3','5','9','5','7','3','5','9','5','9','3','5','9','7','5','3','5','9','7','7','3','5','9','7','9','3',
  '5','9','9','5','3','5','9','9','7','3','5','9','9','9','3','7','3','7','5','3','7','3','7','7','3','7','3','7','9','3','7',
  '3','9','5','3','7','3','9','7','3','7','3','9','9','3','7','5','3','9','3','7','5','5','5','3','7','5','5','7','3','7','5',
  '5','9','3','7','5','7','5','3','7','5','7','7','3','7','5','7','9','3','7','5','9','5','3','7','5','9','7','3','7','5','9',
  '9','3','7','7','3','9','3','7','7','5','5','3','7','7','5','7','3','7','7','5','9','3','7','7','7','5','3','7','7','7','7',
  '3','7','7','7','9','3','7','7','9','5','3','7','7','9','7','3','7','7','9','9','3','7','9','3','9','3','7','9','5','5','3',
  '7','9','5','7','3','7','9','5','9','3','7','9','7','5','3','7','9','7','7','3','7','9','7','9','3','7','9','9','5','3','7',
  '9','9','7','3','7','9','9','9','3','9','3','9','5','3','9','3','9','7','3','9','3','9','9','3','9','5','5','5','3','9','5',
  '5','7','3','9','5','5','9','3','9','5','7','5','3','9','5','7','7','3','9','5','7','9','3','9','5','9','5','3','9','5','9',
  '7','3','9','5','9','9','3','9','7','5','5','3','9','7','5','7','3','9','7','5','9','3','9','7','7','5','3','9','7','7','7',
  '3','9','7','7','9','3','9','7','9','5','3','9','7','9','7','3','9','7','9','9','3','9','9','5','5','3','9','9','5','7','3',
  '9','9','5','9','3','9','9','7','5','3','9','9','7','7','3','9','9','7','9','3','9','9','9','5','3','9','9','9','7','3','9',
  '9','9','9','5','5','5','5','5','7','5','5','5','5','9','5','5','5','7','7','5','5','5','7','9','5','5','5','9','7','5','5',
  '5','9','9','5','5','7','5','7','5','5','7','5','9','5','5','7','7','7','5','5','7','7','9','5','5','7','9','7','5','5','7',
  '9','9','5','5','9','5','7','5','5','9','5','9','5','5','9','7','7','5','5','9','7','9','5','5','9','9','7','5','5','9','9',
  '9','5','7','5','7','7','5','7','5','7','9','5','7','5','9','7','5','7','5','9','9','5','7','7','5','9','5','7','7','7','7',
  '5','7','7','7','9','5','7','7','9','7','5','7','7','9','9','5','7','9','5','9','5','7','9','7','7','5','7','9','7','9','5',
  '7','9','9','7','5','7','9','9','9','5','9','5','9','7','5','9','5','9','9','5','9','7','7','7','5','9','7','7','9','5','9',
  '7','9','7','5','9','7','9','9','5','9','9','7','7','5','9','9','7','9','5','9','9','9','7','5','9','9','9','9','7','7','7',
  '7','7','9','7','7','7','9','9','7','7','9','7','9','7','7','9','9','9','7','9','7','9','9','7','9','9','9','9','9'
};
// Sets up an Advanced Servo object, hooks the event handlers and opens it for device connections.  
// Once an Advanced Servo is attached it will move the motor to various positions, displaying the event details to the console.

int CCONV AttachHandler(CPhidgetHandle ADVSERVO, void *userptr)
{
  int serialNo;
	const char *name;

	CPhidget_getDeviceName (ADVSERVO, &name);
	CPhidget_getSerialNumber(ADVSERVO, &serialNo);
	printf("%s %10d attached!\n", name, serialNo);

	return 0;
}

int CCONV DetachHandler(CPhidgetHandle ADVSERVO, void *userptr)
{
	int serialNo;
	const char *name;

	CPhidget_getDeviceName (ADVSERVO, &name);
	CPhidget_getSerialNumber(ADVSERVO, &serialNo);
	printf("%s %10d detached!\n", name, serialNo);

	return 0;
}

int CCONV ErrorHandler(CPhidgetHandle ADVSERVO, void *userptr, int ErrorCode, const char *Description)
{
	printf("Error handled. %d - %s\n", ErrorCode, Description);
	return 0;
}

int CCONV PositionChangeHandler(CPhidgetAdvancedServoHandle ADVSERVO, void *usrptr, int Index, double Value)
{
	//printf("Motor: %d > Current Position: %f\n", Index, Value);
	return 0;
}
//Display the properties of the attached phidget to the screen.  We will be displaying the name, serial number and version of the attached device.
int display_properties(CPhidgetAdvancedServoHandle phid)
{
	int serialNo, version, numMotors;
	const char* ptr;

	CPhidget_getDeviceType((CPhidgetHandle)phid, &ptr);
	CPhidget_getSerialNumber((CPhidgetHandle)phid, &serialNo);
	CPhidget_getDeviceVersion((CPhidgetHandle)phid, &version);

	CPhidgetAdvancedServo_getMotorCount (phid, &numMotors);

	printf("%s\n", ptr);
	printf("Serial Number: %10d\nVersion: %8d\n# Motors: %d\n", serialNo, version, numMotors);

	return 0;
}

//Pushes the button, then resets the motor to its original position
int resetSecond(CPhidgetAdvancedServoHandle servo){
		CPhidgetAdvancedServo_setPosition (servo, 1, 50.00);
		Sleep(500);
		CPhidgetAdvancedServo_setPosition (servo, 1, 150.00);
		Sleep(500);
		return 0;
	}

int servo_simple(char button,CPhidgetAdvancedServoHandle servo)
{


	
	//change the motor position
	//valid range is -23 to 232, but for most motors ~30-210
	//we'll set it to a few random positions to move it around
	//Step 1: Position 40.00 - also engage servo

	
	//engage all servo motors
	for (i=0;i<2;i++){
	CPhidgetAdvancedServo_setEngaged(servo, i, 1);
	}

	/*
	
	the first set position sets the position of the arm, 
	the second set position actually pushes the button.
	the third set position resets the arm that pushed the button.

	The sleep numbers are very conservative to not run down the hardware too quickly. 
	It takes a little over 7 hours to run through all the numbers completely. 


	*/

	switch(button){
		case  '1':
			CPhidgetAdvancedServo_setPosition (servo, 0, 60.00);
			Sleep(2000);
			printf("Pushing Button 1");
			resetSecond(servo);
			break;
		case  '3':
			 CPhidgetAdvancedServo_setPosition (servo, 0, 70.00);
			Sleep(2000);
			printf("Pushing Button 3");
			resetSecond(servo);
			break;
		case  '5':
			CPhidgetAdvancedServo_setPosition (servo, 0, 80.00);
			Sleep(2000);
			printf("Pushing Button 5");
			resetSecond(servo);
			break;
		case  '7':
			CPhidgetAdvancedServo_setPosition (servo, 0, 90.00);
			Sleep(2000);
			printf("Pushing Button 7");
			resetSecond(servo);
			break;
		case  '9':
			CPhidgetAdvancedServo_setPosition (servo, 0, 100.00);
			Sleep(2000);
			printf("Pushing Button 9");
			resetSecond(servo);
			break;
		default:
			return 0;
	}
	return 0;
}

int main(int argc, char* argv[])
{
int i=0,newstart=0,sleep=0;
int holder=1;
printf("How long to wait every 30 buttons? (Defaults to 60 seconds, enter in seconds)\n");
scanf("%d",sleep);
if ((int)sleep<60){
	sleep=60;
}
printf("Setting Sleep to %d seconds\n",sleep);

int result;
	double curr_pos;
	const char *err;
	double minAccel, maxVel;




	//Declare an advanced servo handle
	CPhidgetAdvancedServoHandle servo = 0;

	//create the advanced servo object
	CPhidgetAdvancedServo_create(&servo);

	//Set the handlers to be run when the device is plugged in or opened from software, unplugged or closed from software, or generates an error.
	CPhidget_set_OnAttach_Handler((CPhidgetHandle)servo, AttachHandler, NULL);
	CPhidget_set_OnDetach_Handler((CPhidgetHandle)servo, DetachHandler, NULL);
	CPhidget_set_OnError_Handler((CPhidgetHandle)servo, ErrorHandler, NULL);

	//Registers a callback that will run when the motor position is changed.
	//Requires the handle for the Phidget, the function that will be called, and an arbitrary pointer that will be supplied to the callback function (may be NULL).
	CPhidgetAdvancedServo_set_OnPositionChange_Handler(servo, PositionChangeHandler, NULL);

	//open the device for connections
	CPhidget_open((CPhidgetHandle)servo, -1);

	//get the program to wait for an advanced servo device to be attached
	printf("Waiting for Phidget to be attached....");
	if((result = CPhidget_waitForAttachment((CPhidgetHandle)servo, 10000)))
	{
		CPhidget_getErrorDescription(result, &err);
		printf("Problem waiting for attachment: %s\n", err);
		return 0;
	}

	//Display the properties of the attached device
	display_properties(servo);

	//read event data
	printf("Reading.....\n");

	//This example assumes servo motor is attached to index 0
	
	//display current motor position
	if(CPhidgetAdvancedServo_getPosition(servo, 0, &curr_pos) == EPHIDGET_OK)
		printf("Motor: 0 > Current Position: %f\n", curr_pos);

	//Set up some initial acceleration and velocity values
	CPhidgetAdvancedServo_getAccelerationMin(servo, 0, &minAccel);
	CPhidgetAdvancedServo_setAcceleration(servo, 0, minAccel*2);
	CPhidgetAdvancedServo_getVelocityMax(servo, 0, &maxVel);
	CPhidgetAdvancedServo_setVelocityLimit(servo, 0, maxVel/2);
	printf("Moving to default positions and engaging servo motor\n");

	CPhidgetAdvancedServo_setPosition (servo, 0, 50.00);
	for (i=1;i<3;i++){
	CPhidgetAdvancedServo_setPosition (servo, i, 150.00);
	}
	Sleep(8000);
	printf("Initializing, please wait...\n");
	Sleep(50);
	while (i<3130){
		for (i;i<3130;i++){
				if (newstart>0){
					i=newstart;
					holder++;
					newstart=0;
					printf("Sleeping for %d seconds\n",sleep);
					Sleep(sleep*1000);
					}
/* 
mod 30* holder(current position in array)=0, set the newstart back by 3 so it will start off retyping the new 
number "primer". It resets at 30 numbers pressed to keep the car from locking you out,which is why sleeps are needed

*/		
			else if ((i%(30*holder)==0)&&(i!=0)){
				newstart=i-3;
			}
			servo_simple(sequence[i], servo);
			printf("curr pos is %d \n",curr_pos);
	
			 }
			system("PAUSE");
			return(0);

		}

//At end, call this to disengage servo and terminate program
	//Step 7: Disengage
	printf("Disengage Servo. Press any key to Continue\n");
	getchar();

	CPhidgetAdvancedServo_setEngaged(servo, 0, 0);

	printf("Press any key to end\n");
	getchar();

	//since user input has been read, this is a signal to terminate the program so we will close the phidget and delete the object we created
	printf("Closing...\n");
	CPhidget_close((CPhidgetHandle)servo);
	CPhidget_delete((CPhidgetHandle)servo);
	

}
	




