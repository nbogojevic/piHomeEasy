#include <wiringPi.h>
#include <iostream>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <sched.h>
#include <sstream>

// Code derived from:
//  http://playground.arduino.cc/Code/HomeEasy
//  idleman@idleman.fr (idleman@idleman.fr - http://blog.idleman.fr)

using namespace std;

#define FOUR_BITS 4
#define TWENTY_SIX_BITS 26
#define MAX_SENDER_ID ((1 << 26) - 1)
#define MAX_RECEIVER_ID 15
#define MAX_PIN 16

void scheduler_realtime() {
	struct sched_param p;
	p.__sched_priority = sched_get_priority_max(SCHED_RR);
	if( sched_setscheduler( 0, SCHED_RR, &p ) == -1 ) {
		perror("Failed to switch to realtime scheduler.");
	}
}

//Send a bit pulse (describes 0 or 1) 
//1 = 310µs high then 1340µs low
//0 = 310µs high then 310µs low
// Encoding is a manchester code  
// See: http://en.wikipedia.org/wiki/Manchester_code
void sendBit(int pin, bool b) 
{
	digitalWrite(pin, HIGH);
	delayMicroseconds(310);   //275 orinally, but tweaked.
	digitalWrite(pin, LOW);
	delayMicroseconds(b ? 1340 : 310);  //1225 orinally, but tweaked.
}

// Data is encoded as two bits when transmitted:
// value 0 = transmitted 01
// value 1 = transmitted 10
void sendPair(int pin, bool b) {
	sendBit(pin, b);
	sendBit(pin, !b);
}

//Sends value as binary of given length 
void sendValue(int pin, unsigned long value, int length)
{
	for(length--; length>=0; length--)
	{
		sendPair(pin, (value & (1 << length)) != 0);
	}
}


// Sends signal
// boolean parameter tels if the device will be turned on or off (true = on, false = off)
void send(int pin, int emitter, int receiver, bool turnOn)
{
	// Do the latch sequence.. 
	digitalWrite(pin, HIGH);
	delayMicroseconds(275);     // bit of radio shouting before we start. 
	digitalWrite(pin, LOW);
	delayMicroseconds(9900);     // low for 9900 for latch 1
	digitalWrite(pin, HIGH);   // high  
	delayMicroseconds(275);    // wait a moment 275µs
	digitalWrite(pin, LOW);    // low again for 2675µs - latch
	delayMicroseconds(2675);
	// End on a high 
	digitalWrite(pin, HIGH);

	// Send emitter code (26 bits, 0-25)
	sendValue(pin, emitter, TWENTY_SIX_BITS);

	// Send group flag (always 0, bit 26)
	// Group activates all receivers associated to an emitter
	// Receiver code of -1 means activate group
	sendPair(pin, receiver == -1);

	// Send command on or off (bit 27) 
	// This command can be sent as 11 (not only as 01 or 10).
	// In that case it is dimmer command, and another 4 bits need to be sent. This is not supported.
	sendPair(pin, turnOn);

	// Send device code as 4 bits (bits 27-31)
	sendValue(pin, receiver >= 0 ? receiver : 0, FOUR_BITS);

	digitalWrite(pin, HIGH);   // Ending latch
	delayMicroseconds(275);    // wait 275µs
	digitalWrite(pin, LOW);    // and finish signal for 2675µs 
}

void usage(char**argv)
{
	cerr << "Usage: " << argv[0] << " <pin number> <emitter id> <receiver id> <on|off>" << endl
  << endl
	<< "  <pin number>  wiringPi pin number as follows: " << endl
	<< "                  number  GPIO|GPIO  number" << endl
	<< "                             1|2           " << endl
	<< "                  8          3|4           " << endl
	<< "                  9          5|6           " << endl
	<< "                  7          7|8         15" << endl
	<< "                             9|10        16" << endl
	<< "                  0         11|12         1" << endl
	<< "                  2         13|14          " << endl
	<< "                  3         15|16         4" << endl
	<< "                            17|18         5" << endl
	<< "                  12        19|20          " << endl
	<< "                  9         21|22         6" << endl
	<< "                  14        23|24        10" << endl
	<< "                            25|26        11" << endl
	<< "  <emitter id>  Unique id of the emitter: a number between 0 and " << MAX_SENDER_ID << "." << endl
  << "                  Example: 12325262" << endl
	<< "  <receiver id> Receiver id: a number between 0 and " << MAX_RECEIVER_ID << ", or -1 for group command." << endl
	<< "                  Group command is received by all receivers paired with the emitter id." << endl
	<< "  <on|off>      Command to send: " << endl 
	<< "                  on  to turn on" << endl
  << "                  off to turn off" << endl 
	<< endl;
}

int main (int argc, char** argv)
{
	// There are 4 parameters, if you don't get them, print usage text
	if (argc != 5)
	{
		usage(argv);
		return 1;
	}
	int pin = atoi(argv[1]);
	int emitter = atoi(argv[2]);
	int receiver = atoi(argv[3]);
	string onoff = argv[4];

	// Validate input parameters. If there is an error, print usage text
	bool hasError = false;
	if (pin > MAX_PIN || pin < 0) {
		cerr << "Invalid argument: pin number should be between 0 and " << MAX_PIN << endl;
		hasError = true;
	}
	if (emitter > MAX_SENDER_ID || emitter < 0) {
		cerr << "Invalid argument: emitter id should be between 0 and " << MAX_SENDER_ID << endl;
		hasError = true;
	}
	if (receiver > MAX_RECEIVER_ID || receiver < -1) {
		cerr << "Invalid argument: receiver should be between -1 and " << MAX_RECEIVER_ID << endl;
		hasError = true;
	}
	if (onoff != "on" && onoff != "off") {
		cerr << "Invalid argument: use either on to turn on the reciever or off to turn it off" << endl;
		hasError = true;
	}
	if (hasError) {
		usage(argv);
		return 1;
	}
	if (setuid(0)) {
		perror("setuid");
    cerr << argv[0] << " must be run as root." << endl;
		return 1;
	}

	// If there is no wiringPi library print instructions for installation.
	if (wiringPiSetup() == -1) {
		cerr << "Wiring Pi library not found, please install it:" << endl
		<< "  cd ~" << endl
		<< "  sudo apt-get install git-core" << endl
		<< "  sudo git clone git://git.drogon.net/wiringPi" << endl
		<< "  cd wiringPi" << endl
		<< "  sudo ./build" << endl;
		return -1;

	}
	bool transmitValue = (onoff=="on"); 
	cout << "Sending command '" << onoff << "' to receiver " << receiver 
    << " by emitter " << emitter 
    << " using pin " << pin << "..." << flush;

	scheduler_realtime();
	pinMode(pin, OUTPUT);
		
	for(int i=0;i<6;i++)
	{
		send(pin, emitter, receiver, transmitValue);   // send signal
		delay(10);             // wait 10 ms, otherwise socket may ignore us
	}

	cout << " finished." << endl;    
  // signal has been sent.
}
