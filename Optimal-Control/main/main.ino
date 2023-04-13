
/* IMPORTED LIBRARIES */
#include <PID_v1.h>
#include <HCSR04.h>

/* MISC CONSTANTS */
#define MAXPULSE 16383 // 14-bit
#define MOTORFREQ 30000
#define LEDFREQ 1
#define PRESETPOINT 1600

/* PINS */
#define MOTORPIN 19
#define IN1PIN 21
#define IN2PIN 18
#define IN1CHANNEL 6
#define IN2CHANNEL 9
#define LEDPIN 15
#define LEDCHANNEL 0
#define SENSOR2PIN 5
#define ECHOPIN 4
#define TRIGGERPIN 2
//#define RESETPIN 
/* LED DUTY CYCLES */
#define TENPERC 6554
#define FORTYPERC 26214
#define ONEHUNPERC 65536
#define SEVENTYPERC 45876

/* CONTROL SIGNALS */
#define RSTN 26
#define OEN 25
#define SEL1 27
#define SEL2 14
#define S1  32
#define S2  33

/* COUNTER SIGNALS */
#define BIT0 35
#define BIT1 34 

volatile long count = 0;

/*
  The algorithm below is based on the Example Routine given in the IC datasheet
*/

/* GLOBAL VARIABLE DECLARATION */
 double setPosition;
 double input, output;
 double Kp = 1, Ki = 0.05, Kd = 0.7;
volatile int pulse = 0;
volatile bool home = true;
volatile int state = 1;
bool reset = true;

/* INITIALIZE PID TIMER, MOTOR PID MODEL, AND SENSOR */
hw_timer_t *pidTimer = NULL;
UltraSonicDistanceSensor distanceSensor(TRIGGERPIN, ECHOPIN);
PID motorPID(&input, &output, &setPosition, Kp, Ki, Kd, DIRECT);

void setup()
{
  Serial.begin(9600);

  // Assign Output Pins
  pinMode(RSTN,OUTPUT);
  pinMode(OEN,OUTPUT);
  pinMode(SEL1,OUTPUT);
  pinMode(SEL2,OUTPUT);
  pinMode(S1,OUTPUT);
  pinMode(S2,OUTPUT);
  pinMode(MOTORPIN, OUTPUT);
  pinMode(IN1PIN, OUTPUT);
  pinMode(IN2PIN, OUTPUT);

  // Assign Input Pins
  pinMode(BIT0,INPUT);
  pinMode(BIT1,INPUT);
  pinMode(SENSOR2PIN, INPUT);
  //pinMode(RESETPIN, INPUT);

  // LED states at 1Hz PWM and 16 bit resolution at channel 0
  ledcAttachPin(LEDPIN, LEDCHANNEL);
  ledcSetup(LEDCHANNEL, LEDFREQ, 16);

  // Motor PWM setup 30kHz at channel 1 and 2 at 8 bit resolution
  ledcAttachPin(IN1PIN, IN1CHANNEL);
  ledcSetup(IN1CHANNEL, MOTORFREQ, 8);
  ledcAttachPin(IN2PIN, IN2CHANNEL);
  ledcSetup(IN2CHANNEL, MOTORFREQ, 8);

  // Initial Decoder Reset
  digitalWrite(RSTN,LOW);
  delay(15);
  digitalWrite(RSTN,HIGH);

  // Initial Control Signals
  digitalWrite(OEN,LOW);
  digitalWrite(SEL1,LOW);
  digitalWrite(SEL2,LOW);
  digitalWrite(S1,LOW);
  digitalWrite(S2,LOW);


  // ISRs
  pidTimer = timerBegin(2, 1, true); // pidTimer number 2, divider = 1 bc no prescalar needed
  timerAttachInterrupt(pidTimer, &PIDFunc, true);
  timerAlarmWrite(pidTimer, 80000, true); // Every 1Khz PID will run 80MHz/80000 = 1Khz
  motorPID.SetMode(AUTOMATIC);

  attachInterrupt(digitalPinToInterrupt(SENSOR2PIN), openClawISR, RISING);
}

void loop()
{
//  if (reset) {
//    while(digitalRead(RESETPIN) == LOW){
//      ledcWrite(IN1CHANNEL, 255);
//      ledcWrite(IN2CHANNEL, 0);
//    }
//    reset = false;
//  }

  switch(state) {
    case 1:
//      timerAlarmEnable(pidTimer);
//      ledcWrite(LEDCHANNEL, SEVENTYPERC);
//   
//      float distance = distanceSensor.measureDistanceCm();
//    
//      Serial.print("Distance from object: ");
//      Serial.println(distance);
//      
//      if (a <= 15 && a >= 0) {
        digitalWrite(MOTORPIN, HIGH);
        Serial.print("Starting close claw");
        delay(5000);
        closeClaw();
//      }
      break;
    case 2:
      travelClaw();
      break;
    case 3:
      openClaw();
      break;
    case 4:
          // read encoder pulse 
      digitalWrite(OEN, HIGH); // Disable OE
      getPulseCount();
  
      Serial.println("Current pulse, input: ");
      Serial.println(count);
      input = abs(setPosition-count);

      Serial.print(input);
  
//      pulse = count;
//      motorPID.Compute();
      Serial.println("Set position, output: ");
      Serial.println(setPosition);
      Serial.println(output);      
      break;
    default:
      Serial.println("Error, no such state");
  }
}

void closeClaw()
{
//  Serial.println("Entering closeClaw state");
  ledcWrite(LEDCHANNEL, TENPERC);
  setPosition = 1600;


  // Start PID
  home = false;
  state = 4;
  timerAlarmEnable(pidTimer);

//  // Should not get to this case, but goes to next state if PID > 30s
//  delay(30000);
//
//  // Next state: Travel Claw
//  travelClaw();
}

void PIDFunc()
{

  if (!home)
  {
    int pin;
    int pin2;

    // Motor running to close (forward)
    if (setPosition < count) {
      pin = IN2CHANNEL;
      pin2 = IN1CHANNEL;
    } else {
      // Motor running to open (backward)
      
      pin = IN1CHANNEL;
      pin2 = IN2CHANNEL;
    }
//      input = abs(setPosition-count);
      pulse = count;
      motorPID.Compute();
    // slow down motor
    ledcWrite(pin, output);
    ledcWrite(pin2, 0);

    // PID settles close enough
    if (output == 0)
    { 
      if (setPosition == 0)
      {
        state = 1;
      }
      else
      {
        state = 2;
      }
      home = true;
    }
  }
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
  state = 4;
  // Next state: openClaw()
//  openClaw();
}

void openClawISR() {
  state = 3;
}

// Last state (ISR):
void openClaw()
{
  Serial.println("Entering openClaw state");
  // ISR sense
  ledcWrite(LEDCHANNEL, ONEHUNPERC);
  setPosition = 0;

  // Start PID
  home = false;
  state = 4;

//  // Should not get to this case, but goes to next state if PID > 30s
//  delay(30000);
//
//  // Next state: Home
//  homeState = true;
//  home = true;
}

/* PLD encoder code*/
byte getByte()
{
byte capture1 = 0; // Initialize a byte variable to read the first capture
for(int i=0; i<4; i++){

  switch(i){
    case 0:
      digitalWrite(S1,LOW);
      digitalWrite(S2,LOW);
      break;
    case 1:
      digitalWrite(S1,HIGH);
      digitalWrite(S2,LOW);
      break;
    case 2:
      digitalWrite(S1,LOW);
      digitalWrite(S2,HIGH);
      break;
    case 3:
      digitalWrite(S1,HIGH);
      digitalWrite(S2,HIGH);
      break; 
    default:
      digitalWrite(S1,LOW);
      digitalWrite(S2,LOW);
      break;
  }

  capture1 |= (digitalRead(BIT0)<<i);
  capture1 |= (digitalRead(BIT1)<<(i+4));
}

byte capture2 = 0; // Initialize a byte variable to read the second capture
for(int i=0; i<4; i++){

  switch(i){
    case 0:
      digitalWrite(S1,LOW);
      digitalWrite(S2,LOW);
      break;
    case 1:
      digitalWrite(S1,HIGH);
      digitalWrite(S2,LOW);
      break;
    case 2:
      digitalWrite(S1,LOW);
      digitalWrite(S2,HIGH);
      break;
    case 3:
      digitalWrite(S1,HIGH);
      digitalWrite(S2,HIGH);
      break; 
    default:
      digitalWrite(S1,LOW);
      digitalWrite(S2,LOW);
      break;
  }

  capture2 |= (digitalRead(BIT0)<<i);
  capture2 |= (digitalRead(BIT1)<<(i+4));
}

if (capture2 == capture1){  // If the first and second capture match then return the result
  byte byteCapture = capture2;
  return byteCapture;  
}
else getByte(); // If the first and second capture DO NOT match, try again to capture 
}

long combine4Bytes(byte byte4, byte byte3, byte byte2, byte byte1)
{
  long combine = 0;
  combine |= (((long)byte4 << 24) | ((long)byte3 << 16) | ((long)byte2 << 8) | ((long)byte1 << 0));
  return combine;
}

void getPulseCount()
{
  delay(25);
  digitalWrite(OEN, HIGH); // Disable OE
  digitalWrite(OEN, LOW); // Enable OE

  // SEL1 = 0 and SEL2 = 1 gets 4th Byte
  digitalWrite(SEL1, LOW);
  digitalWrite(SEL2, HIGH); 
  byte Byte4 = getByte();

  // SEL1 = 1 and SEL2 = 1 gets 3rd Byte
  digitalWrite(SEL1, HIGH);
  digitalWrite(SEL2, HIGH); 
  byte Byte3 = getByte();
  
  // SEL1 = 0 and SEL2 = 0 gets 2nd Byte
  digitalWrite(SEL1, LOW);
  digitalWrite(SEL2, LOW); 
  byte Byte2 = getByte();
  
  // SEL1 = 1 and SEL2 = 0 gets 1st Byte
  digitalWrite(SEL1, HIGH);
  digitalWrite(SEL2, LOW); 
  byte Byte1 = getByte();

  digitalWrite(OEN, HIGH); // disable OE
  delay(25);
  count = combine4Bytes(Byte4,Byte3,Byte2,Byte1);
}
