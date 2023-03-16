#include <HCSR04.h>

#define PLDCOMM 10
#define MOTORPIN 10
#define IN1PIN 11
#define IN2PIN 12
#define LEDPIN 32
#define LEDCHANNEL 0
#define SENSOR2PIN 33
#define ECHOPIN 26
#define TRIGGERPIN 27

#define TENPERC 6554
#define FORTYPERC 26214
#define ONEHUNPERC 65536
#define SEVENTYPERC 45876

/* global variable declaration */
int pulse = 0b0;
bool home = true;
hw_timer_t* pidTimer = NULL;
UltraSonicDistanceSensor distanceSensor(3, 4); //Initialize sensor

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(MOTORPIN, OUTPUT);
  pinMode(IN1PIN, OUTPUT);
  pinMode(IN2PIN, OUTPUT);
  pinMode(SENSOR2PIN,INPUT);

  //LED states at 1Hz PWM and 16 bit resolution at channel 0
  ledcAttachPin(LEDPIN, LEDCHANNEL);
  ledcSetup(LEDCHANNEL, 1, 16);

  //ISRs
  //This lets timer1 frequency 80MHz/80kHz=1Khz
  hw_timer_t* pidTimer = timerBegin(2, 80000, true);
  timerAttachInterrupt(pidTimer, &PID, true);
  //Every 500ms PID will run
  timerAlarmWrite(pidTimer, 500 ,true);
  timerAlarmEnable(pidTimer);

  attachInterrupt(digitalPinToInterrupt(SENSOR2PIN), openClaw, RISING);
}

void loop()
{
  ledcWrite(LEDCHANNEL, SEVENTYPERC);
  float distance = distanceSensor.measureDistanceCm();
  
  if (distance <= 15.0 && distance >= 0&& home) {
    closeClaw();
  }
}

void closeClaw()
{
  ledcWrite(LEDCHANNEL, TENPERC);

  // PIN D10-D13
  digitalWrite(MOTORPIN, HIGH);

  // Motor spin forward
  digitalWrite(IN1PIN, HIGH);
  digitalWrite(IN2PIN, LOW);

  home = false;

  // Next state: Travel Claw
  travelClaw();
}

void PID() {
  if !home {
    int pulsetemp = 0b0;

    for (int i = 0; i < PLDCOMM; i++)
    {
      // PIN D0 - D9 (D0-D9)
      pulsetemp = pulsetemp << 1 | int(digitalRead(i));
    }
    // Calculate error, return difference 
  }
}

void travelClaw()
{
  home = true;
  ledcWrite(LEDCHANNEL, FORTYPERC);
  
  // Motor stop spin
  digitalWrite(IN1PIN, LOW);
  digitalWrite(IN2PIN, LOW);
}

// Last state (ISR): 
void openClaw()
{
  // ISR sense
  ledcWrite(LEDCHANNEL, ONEHUNPERC);

  // PIN D10-D13
  digitalWrite(MOTORPIN, HIGH);

  home == false;

  // Motor spin forward
  // Do this until a certain point
  digitalWrite(IN1PIN, LOW);
  digitalWrite(IN2PIN, HIGH);
 
  // End
  digitalWrite(IN1PIN, LOW);
  digitalWrite(IN2PIN, LOW);
}
