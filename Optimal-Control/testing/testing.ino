//#include <TimerOne.h>
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
hw_timer_t* timer1 = NULL;
UltraSonicDistanceSensor distanceSensor(TRIGGERPIN, ECHOPIN); //Initialize sensor

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);

  // pinMode(MOTORPIN, OUTPUT);
  // pinMode(IN1PIN, OUTPUT);
  // pinMode(IN2PIN, OUTPUT);
  // pinMode(SENSOR2PIN,INPUT);

  //LED states
  ledcAttachPin(LEDPIN, LEDCHANNEL);
  ledcSetup(LEDCHANNEL, 1, 16);

  //Internal ISRs
  // This lets timer1 frequency 80MHz/80kHz=1Khz
  hw_timer_t* timer1 = timerBegin(1, 80000, true);
  timerAttachInterrupt(timer1, &closeClaw, true);
  //Every 500ms PID will run
  timerAlarmWrite(timer1, 500 ,true);
  timerAlarmEnable(timer1);

  // attachInterrupt(digitalPinToInterrupt(SENSOR1PIN,closeClaw, RISING);
   attachInterrupt(digitalPinToInterrupt(SENSOR2PIN), openClaw, RISING);
}

void loop()
{
//  ledcWrite(LEDCHANNEL, SEVENTYPERC);
//  Serial.println(.7);
//  delay(2000);
  // Next state: Close claw
  // if (distanceSensor.measureDistanceCm() <= 10 && home) {
    Serial.println(.1);
    ledcWrite(LEDCHANNEL, TENPERC);
    float a = distanceSensor.measureDistanceCm();
    Serial.println(a);
    if (a <= 15 && a >= 0) {
      ledcWrite(LEDCHANNEL, ONEHUNPERC);
      delay(2000);
    }
    delay(2000);
    // Timer1.setPwmDuty(LEDPIN, TENPERC);
//    closeClaw();
  // } 
}

void closeClaw()
{
  // int pulsetemp = 0b0;

  // for (int i = 0; i < PLDCOMM; i++)
  // {
  //   // PIN D0 - D9 (D0-D9)
  //   pulsetemp = pulsetemp << 1 | int(digitalRead(i));
  // }

  //compare pulsetemp with pulse

  // PIN D10-D13
//  digitalWrite(MOTORPIN, HIGH);
// analogWrite(MOTORPIN, 255);

//   // Motor spin forward
//   digitalWrite(IN1PIN, HIGH);
//   digitalWrite(IN2PIN, LOW);

//   PID();

  // Next state: Travel Claw
  
//  travelClaw();
Serial.println("Internal PID works woo");
  ledcWrite(LEDCHANNEL, FORTYPERC);
}

void PID() {
  //TBD
}

void travelClaw()
{
//  delay(2000);
  // Timer1.setPwmDuty(LEDPIN, FORTYPERC);
//  Serial.println(.4);
//Serial.println("Internal PID works woo");
//  ledcWrite(LEDCHANNEL, FORTYPERC);
//  openClaw();
  // Motor stop spin
  // digitalWrite(IN1PIN, LOW);
  // digitalWrite(IN2PIN, LOW);

  // home = true;
}

// Last state (ISR): 
void openClaw()
{
//  delay(2000);
  // ISR sense
  Serial.println("Magnetic sensor works woo");
//  ledcWrite(LEDCHANNEL, ONEHUNPERC);
//  delay(2000);
  // Timer1.setPwmDuty(LEDPIN, ONEHUNPERC);

  // PIN D10-D13
//  digitalWrite(MOTORPIN, HIGH);
  // analogWrite(MOTORPIN, 125);

  // // Motor spin forward
  // digitalWrite(IN1PIN, LOW);
  // digitalWrite(IN2PIN, HIGH);


  //Do this until certain point reached
  // while pulsetemp != pulse {
  //   for (int i = 0; i < PLDCOMM; i++)
  //   {
  //     // PIN D0 - D9 (D0-D9)
  //     pulsetemp = pulsetemp << 1 | int(digitalRead(i));
  //   }
  // }

  // digitalWrite(IN1PIN, LOW);
  // digitalWrite(IN2PIN, LOW);
  // home == false; // returns home
}
