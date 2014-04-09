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
//int motorSpeed = 125;

int sensorValue = 0;
const int sensorRange = 50;	// what is this in cm?
const boolean debug = false;

// Initialization
void setup()
{
	if( debug )
		Serial.begin(baudRate);
	pinMode(lMotor, OUTPUT);
	pinMode(rMotor, OUTPUT);
	setMotor(LOW,LOW);
}

// Main frame of execution
void loop()
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
		setMotor(HIGH,HIGH);
		turnBool = false;
	}
}

// Write digital value to H-Bridge (DC motor)
void setMotor(int lState, int rState)
{
	digitalWrite(lMotor,lState);
	digitalWrite(rMotor,rState);
}
