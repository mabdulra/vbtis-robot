/**
 * Video-Based Target Identification System
 * Robot Subsystem (Blumenkranz)
 * 
 * Move forward while reading from sonar sensor
 * If within distance range, check left and right sonar
 * Move in direction with most room until forward is free
 *
 * Override autonomous control if Pi says so
 * Communicate with Pi over SPI
 * Light LED entirely based on whim of the Pi
 * 
 * (c) 2014 Muhammad Abdul-Rahim
 */

#include <SPI.h>

//	Self-defined SPI bytes
#define MOTOR_START		0x01
#define MOTOR_STOP		0x02
#define LED_ON			0x03
#define LED_OFF			0x04

//	Motor states
#define MOTION_NONE		0x00
#define MOTION_FORWARD	0x01
#define MOTION_LEFT		0x02
#define MOTION_RIGHT	0x03

//	Analog pins for sonar sensors
const int fSonar = A0;
const int lSonar = A1;
const int rSonar = A2;

//	Digital pins for H-Bridge logic
const int lMotorFront = 2;
const int lMotorBack = 3;
const int rMotorFront = 4;
const int rMotorBack = 5;

//	Digital pin for LED
const int ledPin = 7;

//	Debug variables
const boolean debug = true;
const int baudRate = 9600;

//	Motor control variables
const int sensorRange = 50;	// what is this in cm?
const int motorDelay = 300;		// milliseconds

//	Logical booleans
volatile boolean motorsAllowed = true;
boolean inTurn = false;

//	read all sonar sensors at the start
unsigned int fSonarValue;
unsigned int lSonarValue;
unsigned int rSonarValue;

void setup()
{
	if( debug )
		Serial.begin(baudRate);
	
	//	initialize pins
	pinMode(lMotorFront,	OUTPUT);
	pinMode(lMotorBack,		OUTPUT);
	pinMode(rMotorFront,	OUTPUT);
	pinMode(rMotorBack,		OUTPUT);
	pinMode(ledPin,			OUTPUT);
	setMotors(MOTION_NONE);
	
	// initialize SPI
	pinMode(MISO,OUTPUT);
	SPCR |= _BV(SPE);
	SPI.attachInterrupt();
}

//	SPI Interrupt Service Routine
ISR(SPI_STC_vect)
{
	byte b = SPDR;	// SPI Data Register
	if( debug )
		Serial.println(b,HEX);
	
	switch(b)
	{
		//	Motor control dictated by main loop()
		case MOTOR_START:
			motorsAllowed = true;
			break;
		case MOTOR_STOP:
			motorsAllowed = false;
			setMotors(MOTION_NONE);
			break;
		
		//	LED control self-contained
		case LED_ON:
			digitalWrite(ledPin,HIGH);
			break;
		case LED_OFF:
			digitalWrite(ledPin,LOW);
			break;
	}
}

void loop()
{
	if( !motorsAllowed )
		setMotors(MOTION_NONE);
	else
	{
		//	read all sonar sensors at the start
		fSonarValue = analogRead(fSonar);
		lSonarValue = analogRead(lSonar);
		rSonarValue = analogRead(rSonar);
		
		//	I wish prinf() could work for this, but alas
		if( debug )
		{
			Serial.print("{");
			Serial.print(fSonarValue);
			Serial.print(",");
			Serial.print(lSonarValue);
			Serial.print(",");
			Serial.print(rSonarValue);
			Serial.println("}");
		}
		
		//	if the front sonar is blocked, handle directional turn
		if( fSonarValue < sensorRange )
		{
			//	force a slight delay before starting turn
			if( !inTurn )
			{
				setMotors(MOTION_NONE);
				delay(motorDelay);
				inTurn = true;
			}
			
			//	determine direction to turn in and turn
			if( rSonarValue > lSonarValue )
				setMotors(MOTION_RIGHT);
			else
				setMotors(MOTION_LEFT);
		}
		else
		{
			setMotors(MOTION_FORWARD);
			inTurn = false;
		}
		
		//	slow down the robot
		delay(motorDelay/4);
		setMotors(MOTION_NONE);
		delay(motorDelay);
		
	}
}

//	Set motor based on which direction you want to go (obfuscation!)
void setMotors(byte state)
{
	switch(state)
	{
		case MOTION_NONE:
			digitalWrite(lMotorFront,	LOW);
			digitalWrite(lMotorBack,	LOW);
			digitalWrite(rMotorFront,	LOW);
			digitalWrite(rMotorBack,	LOW);
			break;
		case MOTION_FORWARD:
			digitalWrite(lMotorFront,	HIGH);
			digitalWrite(lMotorBack,	LOW);
			digitalWrite(rMotorFront,	HIGH);
			digitalWrite(rMotorBack,	LOW);
			break;
		case MOTION_LEFT:
			digitalWrite(lMotorFront,	LOW);
			digitalWrite(lMotorBack,	HIGH);
			digitalWrite(rMotorFront,	HIGH);
			digitalWrite(rMotorBack,	LOW);
			break;
		case MOTION_RIGHT:
			digitalWrite(lMotorFront,	HIGH);
			digitalWrite(lMotorBack,	LOW);
			digitalWrite(rMotorFront,	LOW);
			digitalWrite(rMotorBack,	HIGH);
			break;
	}
}
