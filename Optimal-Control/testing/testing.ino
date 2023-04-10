
/* IMPORTED LIBRARIES */
#include <PID_v1.h>
#include <HCSR04.h>

/* MISC CONSTANTS */
#define PLDCOMM 10
#define MAXPULSE 16383 // 14-bit
#define MOTORFREQ 400
#define LEDFREQ 1
#define PRESETPOINT 8544

/* PINS */
#define MOTORPIN 14
#define IN1PIN 27
#define IN2PIN 12
#define IN1CHANNEL 6
#define IN2CHANNEL 9
#define LEDPIN 21
#define LEDCHANNEL 0
#define SENSOR2PIN 33
#define ECHOPIN 19
#define TRIGGERPIN 18

/* LED DUTY CYCLES */
#define TENPERC 6554
#define FORTYPERC 26214
#define ONEHUNPERC 65536
#define SEVENTYPERC 45876

/* global variable declaration */
int pulse = 0b0;
bool home = true;
hw_timer_t* timer1 = NULL;
UltraSonicDistanceSensor distanceSensor(TRIGGERPIN, ECHOPIN);//Initialize sensor

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);

   pinMode(MOTORPIN, OUTPUT);
   pinMode(IN1PIN, OUTPUT);
   pinMode(IN2PIN, OUTPUT);
//   pinMode(SENSOR2PIN,INPUT);

  //LED states
  ledcAttachPin(LEDPIN, LEDCHANNEL);
  ledcSetup(LEDCHANNEL, LEDFREQ, 16);

    // Motor PWM setup 30kHz at channel 1 and 2 at 8 bit resolution
  ledcAttachPin(IN1PIN, IN1CHANNEL);
  ledcSetup(IN1CHANNEL, MOTORFREQ, 8);
  ledcAttachPin(IN2PIN, IN2CHANNEL);
  ledcSetup(IN2CHANNEL, MOTORFREQ, 8);

  //Internal ISRs
//  // This lets timer1 frequency 80MHz/80kHz=1Khz
//  hw_timer_t* timer1 = timerBegin(1, 80000, true);
//  timerAttachInterrupt(timer1, &closeClaw, true);
  //Every 500ms PID will run
//  timerAlarmWrite(timer1, 500 ,true);
//  timerAlarmEnable(timer1);

//   attachInterrupt(digitalPinToInterrupt(SENSOR2PIN), openClaw, RISING);
}

void loop()
{
    ledcWrite(LEDCHANNEL, SEVENTYPERC);
    float a = distanceSensor.measureDistanceCm();
    Serial.println(a); 
    if (a <= 15 && a >= 0) {
      delay(2000);
      closeClaw();
    }
    delay(2000);
}

void closeClaw()
{
  Serial.println("Entering closeClaw state");
  ledcWrite(LEDCHANNEL, TENPERC);
  
  digitalWrite(MOTORPIN, HIGH);
   ledcWrite(IN1CHANNEL, 0);
  ledcWrite(IN2CHANNEL, 50);
  delay(3000);
  travelClaw();
}

void PID() {
  //TBD
}

void travelClaw()
{
  Serial.println("Entering travelClaw state");
  home = true;
  ledcWrite(LEDCHANNEL, FORTYPERC);

  // Motor stop spin, sanity check
  ledcWrite(IN1CHANNEL, 0);
  ledcWrite(IN2CHANNEL, 0);

  // Should not get to this case, but goes to next state if
  // it doesn't see sensor for > 30s
  delay(3000);

  // Next state: openClaw()
  openClaw();
}

// Last state (ISR): 
void openClaw()
{
  Serial.println("Entering openClaw state");
  // ISR sense
  ledcWrite(LEDCHANNEL, ONEHUNPERC);

   ledcWrite(IN1CHANNEL, 50);
  ledcWrite(IN2CHANNEL, 0);
  delay(3000);

   ledcWrite(IN1CHANNEL, 0);
  ledcWrite(IN2CHANNEL, 0);
}
