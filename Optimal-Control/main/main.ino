#include <TimerOne.h>
#include <HCSR04.h>

#define PLDCOMM 10
#define MOTORPIN 10
#define IN1PIN 11
#define IN2PIN 12
#define LEDPIN 19
#define SENSOR1PIN 
#define SENSOR2PIN 5

#define TENPERC 102
#define FORTYPERC 409
#define ONEHUNPERC 1023
#define SEVENTYPERC 716

/* global variable declaration */
int pulse = 0b0;
bool home = true;

UltraSonicDistanceSensor distanceSensor(3, 4); //Initialize sensor

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(MOTORPIN, OUTPUT);
  pinMode(IN1PIN, OUTPUT);
  pinMode(IN2PIN, OUTPUT);
  pinMode(LEDPIN, OUTPUT);
  pinMode(SENSOR2PIN,INPUT);

  //LED states
  Timer1.initialize(1000000);
  Timer1.pwm(LEDPIN, SEVENTYPERC);

  // attachInterrupt(digitalPinToInterrupt(SENSOR1PIN,closeClaw, RISING);
  attachInterrupt(digitalPinToInterrupt(SENSOR2PIN), openClaw, RISING);
}

void loop()
{
  // Next state: Close claw
  if (distanceSensor.measureDistanceCm() <= 10 && home) {
    Timer1.setPwmDuty(LEDPIN, TENPERC);
    closeClaw();
  } 
}

void closeClaw()
{
  int pulsetemp = 0b0;

  for (int i = 0; i < PLDCOMM; i++)
  {
    // PIN D0 - D9 (D0-D9)
    pulsetemp = pulsetemp << 1 | int(digitalRead(i));
  }

  //compare pulsetemp with pulse

  // PIN D10-D13
  digitalWrite(MOTORPIN, HIGH);

  // Motor spin forward
  digitalWrite(IN1PIN, HIGH);
  digitalWrite(IN2PIN, LOW);

  PID();

  // Next state: Travel Claw
  travelClaw();
}

void PID() {
  //TBD
}

void travelClaw()
{
  Timer1.setPwmDuty(LEDPIN, FORTYPERC);
  // Motor stop spin
  digitalWrite(IN1PIN, LOW);
  digitalWrite(IN2PIN, LOW);

  home = true;
}

// Last state (ISR): 
void openClaw()
{
  // ISR sense
  Timer1.setPwmDuty(LEDPIN, ONEHUNPERC);

  // PIN D10-D13
  digitalWrite(MOTORPIN, HIGH);

  // Motor spin forward
  digitalWrite(IN1PIN, LOW);
  digitalWrite(IN2PIN, HIGH);


  //Do this until certain point reached
  // while pulsetemp != pulse {
  //   for (int i = 0; i < PLDCOMM; i++)
  //   {
  //     // PIN D0 - D9 (D0-D9)
  //     pulsetemp = pulsetemp << 1 | int(digitalRead(i));
  //   }
  // }

  digitalWrite(IN1PIN, LOW);
  digitalWrite(IN2PIN, LOW);
  home == false; // returns home
}
