            /*
Tim Michaud

tmm08a@acu.edu

PhidgetCarUnlock
*/

#include <stdio.h>
#include <phidget21.h>
#include <windows.h>
#include <iostream>
#include <map>
using namespace std;
map<char, double> mapCaseToPosition;
/*
Explanation and number sequence here: http://everything2.com/title/Weak+security+in+our+daily+lives
TL;DR of link:

Car Code is (assumed) to be 5 numbers long (current standard in Ford/Chevy/Dodge and possibly Toyota cars)

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


#define SLEEP_LENGTH                         60  /*number of SECONDS to sleep to circumvent the auto-lockout on the car*/
#define MAX_NUMBER_OF_INPUTS_BEFORE_RESET    30  /*number of numbers to enter before sleeping for the SLEEP_LENGTH*/
#define MAX_NUMBER_OF_COMBINATIONS           3130 /*max number of possible combinations for a door to be tried*/

/*
Modify these to change the location/placement on your control board. If you plug the arm into the top one and the servo
motor right below it, these defaults will work just fine.
*/

#define ARM_MOTORNUMBER                      0  /*position of the arm on the servo control board*/
#define SERVO_MOTORNUMBER                    1  /*position of the servo motor on the servo control board*/
/*
Modify these to change so the arm will line up with the location of the buttons on your car
FROM 0-180.00
*/
#define FIRST_BUTTON_LOCATION_FOR_ARM        60.00  /*location (in degrees) of the 0/1 or 1/2 button*/
#define SECOND_BUTTON_LOCATION_FOR_ARM       70.00  /*location (in degrees) of the 2/3 or 3/4 button*/
#define THIRD_BUTTON_LOCATION_FOR_ARM        80.00  /*location (in degrees) of the 4/5 or 5/6 button*/
#define FOURTH_BUTTON_LOCATION_FOR_ARM       90.00  /*location (in degrees) of the 6/7 or 7/8 button*/
#define FIFTH_BUTTON_LOCATION_FOR_ARM        100.00  /*location (in degrees) of the 8/9 or 9/0 button*/

/*
Modify this if you want the arm to go back more than 3 (to retry a number, or if your car code is not 5 numbers long)

If your car code is 4 numbers - set this to 2 
*/

#define PRIMER_RESET                         3  /*number of positions in the array needing to go back after a reset due to MAX_NUMBER_OF_INPUTS_BEFORE_RESET*/


/*
WARNING!!!!!!!!!!!!!!!
DO NOT CHANGE ACUTAL_BUTTON_PRESS and RESET_ARM_FROM_BUTTON_PRESS, WILL BREAK THE SERVO MOTOR from over and under extending
TIME_FOR_ARM_TO_MOVE is to give the arm sufficient time to move into position to hit the correct button. 4 seconds is the default,
works well in most cases. With better hardware this could be sped up significantly

Also, do not change sequence[].
*/
#define DEFAULT_WAIT                         10000 /*number of milliseconds to wait for attachments to the Phidget motor*/
#define ACUTAL_BUTTON_PRESS                  50.00  /*number of degrees the servo motor needs to rotate to press the button in*/
#define RESET_MOTOR_FROM_BUTTON_PRESS        150.00  /*number of degrees that the servo motor needs to rotate to release the button and reset itself*/
#define TIME_FOR_ARM_TO_MOVE                 4  /*number of SECONDS to sleep to give arm time to move into position*/
#define MOTOR_ON                             1 /*Turns servo device on*/
#define MOTOR_OFF                            0 /*Turns servo device off*/
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
  int result=0;
  int serialNo=0;
  const char *name="";

  CPhidget_getDeviceName (ADVSERVO, &name);
  CPhidget_getSerialNumber(ADVSERVO, &serialNo);
  printf("%s %10d attached!\n", name, serialNo);

  return result;
}

int CCONV DetachHandler(CPhidgetHandle ADVSERVO, void *userptr)
{
		
  int serialNo=0,result=0;
  const char *name="";

  CPhidget_getDeviceName (ADVSERVO, &name);
  CPhidget_getSerialNumber(ADVSERVO, &serialNo);
  printf("%s %10d detached!\n", name, serialNo);

  return result;
}

int CCONV ErrorHandler(CPhidgetHandle ADVSERVO, void *userptr, int ErrorCode, const char *Description)
{
  int result=0;
  printf("Error handled. %d - %s\n", ErrorCode, Description);
  return result;
}

int CCONV PositionChangeHandler(CPhidgetAdvancedServoHandle ADVSERVO, void *usrptr, int Index, double Value)
{
  int result=0;
  //printf("Motor: %d > Current Position: %f\n", Index, Value);
  return result;
}
//Display the properties of the attached phidget to the screen.  We will be displaying the name, serial number and version of the attached device.
int display_properties(CPhidgetAdvancedServoHandle phid)
{
  int serialNo=0, version=0, numMotors=0,result=0;
  const char* ptr="";

  CPhidget_getDeviceType((CPhidgetHandle)phid, &ptr);
  CPhidget_getSerialNumber((CPhidgetHandle)phid, &serialNo);
  CPhidget_getDeviceVersion((CPhidgetHandle)phid, &version);

  CPhidgetAdvancedServo_getMotorCount (phid, &numMotors);

  printf("%s\n", ptr);
  printf("Serial Number: %10d\nVersion: %8d\n# Motors: %d\n", serialNo, version, numMotors);

  return result;
}

//Pushes the button, then resets the motor to its original position
int PushAndResetSecond(CPhidgetAdvancedServoHandle servo){
  int result=0;
  CPhidgetAdvancedServo_setPosition (servo, SERVO_MOTORNUMBER, ACUTAL_BUTTON_PRESS);
  Sleep(500);
  CPhidgetAdvancedServo_setPosition (servo, SERVO_MOTORNUMBER, RESET_MOTOR_FROM_BUTTON_PRESS);
  Sleep(500);
  return result;
}

int servo_simple(char button,CPhidgetAdvancedServoHandle servo)
{
  int i=0,result=0;

	
	//change the motor position
	//valid range is -23 to 232, but for most motors ~30-210
	//we'll set it to a few random positions to move it around
	//Step 1: Position 40.00 - also engage servo

	/*
	
	the first set position sets the position of the arm, 
	the second set position actually pushes the button.
	the third set position resets the arm that pushed the button.

	The sleep numbers are very conservative to not run down the hardware too quickly. 
	It takes a little over 2 hours to run through all the numbers completely. 


	*/

  // for error handling, making sure we know about this button
  map<char, double>::iterator it = mapCaseToPosition.find( button );
  // note the variable on the right of the expression – not the intuitive way to think about it, but avoids accidental assignment
  if( mapCaseToPosition.end() != it ){
    // catch return value here?
    CPhidgetAdvancedServo_setPosition (servo, ARM_MOTORNUMBER, it->second);
    Sleep(TIME_FOR_ARM_TO_MOVE*1000);
    printf("Pushing Button %c", button);
    // catch return here, too?
    PushAndResetSecond(servo);
  }
  else{
    printf( "Invalid input: %c", button );
  }
  return result;
}

int main(int argc, char* argv[])
{
  int i=0,newstart=0,result=0;
  printf("Setting Sleep to %d seconds\n",SLEEP_LENGTH);
  double curr_pos=0.00;
  const char *err="";
  double minAccel=0.00, maxVel=0.00;

  mapCaseToPosition['1'] = FIRST_BUTTON_LOCATION_FOR_ARM;
  mapCaseToPosition['3'] = SECOND_BUTTON_LOCATION_FOR_ARM; 
  mapCaseToPosition['5'] = THIRD_BUTTON_LOCATION_FOR_ARM;
  mapCaseToPosition['7'] = FOURTH_BUTTON_LOCATION_FOR_ARM;  
  mapCaseToPosition['9'] = FIFTH_BUTTON_LOCATION_FOR_ARM;

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
  result = CPhidget_waitForAttachment((CPhidgetHandle)servo, DEFAULT_WAIT);
  if( EPHIDGET_OK != result ){
 	  int result2=0;
    CPhidget_getErrorDescription(result, &err);
   	printf("Problem waiting for attachment: %s\n", err);
   	return result2;
  }

  //Display the properties of the attached device
  display_properties(servo);

  //read event data
  printf("Reading.....\n");

  //This example assumes servo motor is attached to index 0
	
  //display current motor position
  if(CPhidgetAdvancedServo_getPosition(servo, ARM_MOTORNUMBER, &curr_pos) == EPHIDGET_OK){
	  printf("Motor: %d > Current Position: %f\n",ARM_MOTORNUMBER, curr_pos);
  }
  //Set up some initial acceleration and velocity values
  CPhidgetAdvancedServo_getAccelerationMin(servo, ARM_MOTORNUMBER, &minAccel);
  CPhidgetAdvancedServo_setAcceleration(servo, ARM_MOTORNUMBER, minAccel*2);
  CPhidgetAdvancedServo_getVelocityMax(servo, ARM_MOTORNUMBER, &maxVel);
  CPhidgetAdvancedServo_setVelocityLimit(servo, ARM_MOTORNUMBER, maxVel/2);
  printf("Moving to default positions and engaging servo motor\n");

  CPhidgetAdvancedServo_setPosition (servo, ARM_MOTORNUMBER, FIRST_BUTTON_LOCATION_FOR_ARM);
  CPhidgetAdvancedServo_setEngaged(servo, ARM_MOTORNUMBER, MOTOR_ON);
  
  CPhidgetAdvancedServo_setPosition (servo, SERVO_MOTORNUMBER, RESET_MOTOR_FROM_BUTTON_PRESS);
  CPhidgetAdvancedServo_setEngaged(servo, SERVO_MOTORNUMBER, MOTOR_ON);
  //pause in case arm is severely out of position
  printf("Initializing, please wait...\n");
  Sleep(4000);
  for (i;i<MAX_NUMBER_OF_COMBINATIONS;i++){
	if (newstart>0){
	  i=newstart;
	  newstart=0;
	  printf("Sleeping for %d seconds\n",SLEEP_LENGTH);
	  Sleep(SLEEP_LENGTH*1000);
	}

//Every 30 numbers the car locks the person out (so one can't guess the code too quickly)
//To circumvent this, every 30 numbers we set it back by 3 positions to rebuild our "primer", and start over again

	else if ((i!=0)&&(i%MAX_NUMBER_OF_INPUTS_BEFORE_RESET==0)){
	  newstart=i-PRIMER_RESET;
	}
	  servo_simple(sequence[i], servo);
	  printf("curr pos is %d \n",curr_pos);
	}

//At end, call this to disengage servo and terminate program
  //Step 7: Disengage
  printf("Disengage Servo. Press any key to Continue\n");
  getchar();
  
  CPhidgetAdvancedServo_setEngaged(servo, ARM_MOTORNUMBER, MOTOR_OFF);
  CPhidgetAdvancedServo_setEngaged(servo, SERVO_MOTORNUMBER, MOTOR_OFF);

  printf("Press any key to end\n");
  getchar();

  //since user input has been read, this is a signal to terminate the program so we will close the phidget and delete the object we created
  printf("Closing...\n");
  CPhidget_close((CPhidgetHandle)servo);
  CPhidget_delete((CPhidgetHandle)servo);
}
