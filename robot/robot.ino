/**
 * Video-Based Target Identification System
 * Robot Subsystem (Blumenkranz)
 * 
 * Move forward while reading from sonar sensor
 * If within distance range, check left and right sonar
 * Move in direction with most room until forward is free
 * 
 * mabdulra.net
 * (c) 2014 Muhammad Abdul-Rahim
 */

#include <SPI.h>

#define MOTOR_START	0x02
#define MOTOR_STOP	0x01

// Analog pins for sonar sensors
int fSonar = A0;
int lSonar = A1;
int rSonar = A2;

// Digital pins for H-Bridge Logic (DC motors)
int lMotor = 52;
int rMotor = 50;

boolean turnBool = false;
int baudRate = 9600;

int motorDelay = 300;	// milliseconds

int sensorValue = 0;
const int sensorRange = 50;	// what is this in cm?
const boolean debug = true;

volatile boolean motorsAllowed = true;

void setup()
{
	// initialize pins and motors
	if( debug )
		Serial.begin(baudRate);
	pinMode(lMotor, OUTPUT);
	pinMode(rMotor, OUTPUT);
	setMotor(LOW,LOW);
	
	// initialize SPI
	
}

// Interrupt Service Routing (ISR) for Serial Port Interface (SPI)
ISR(SPI_STC_vect)
{
	byte b = SPDR;	// read byte from SPI Data Register
	
	if( b==MOTOR_START )
		motorsAllowed = true;
	else if( b==MOTOR_STOP )
		motorsAllowed = false;
}

void loop()
{
	if( !motorsAllowed )
		setMotor(LOW,LOW);	// do not move
	else
	{
		// Read front-facing sonar sensor
		sensorValue = analogRead(fSonar);
	
		if( debug )
		{
			Serial.println(sensorValue);
			delay(motorDelay/2);
		}
		
		// If front-facing sonar is blocked, handle directional turns
		if( sensorValue <= sensorRange )
		{
			if( !turnBool )
			{
				setMotor(LOW,LOW);
				delay(motorDelay);	// stop before turning
				turnBool = true;
			}
			
			// Determine turning direction
			if( analogRead(rSonar) > analogRead(lSonar) )
				setMotor(HIGH,LOW);	// turning right (left=HIGH, right=LOW)
			else
				setMotor(LOW,HIGH);	// turning left (left=LOW, right=HIGH)
		}
		else
		{
			setMotor(HIGH,HIGH);		// move forward
			turnBool = false;
		}
	}
}

// Write digital value to H-Bridge (DC motor)
void setMotor(int lState, int rState)
{
	digitalWrite(lMotor,lState);
	digitalWrite(rMotor,rState);
}
