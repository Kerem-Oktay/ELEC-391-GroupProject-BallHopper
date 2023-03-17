
/* IMPORTED LIBRARIES */
#include <PID_v1.h>
#include <HCSR04.h>

/* MISC CONSTANTS */
#define PLDCOMM 10
#define MAXPULSE 16383 //14-bit
#define MOTORFREQ 30000
#define LEDFREQ 1
#define PRESETPOINT 8544

/* PINS */
#define MOTORPIN 10
#define IN1PIN 11
#define IN2PIN 12
#define IN1CHANNEL 1
#define IN2CHANNEL 1
#define LEDPIN 32
#define LEDCHANNEL 0
#define SENSOR2PIN 33
#define ECHOPIN 26
#define TRIGGERPIN 27

/* LED DUTY CYCLES */
#define TENPERC 6554
#define FORTYPERC 26214
#define ONEHUNPERC 65536
#define SEVENTYPERC 45876

/* GLOBAL VARIABLE DECLARATION */
double setPosition;
double input, output;
double Kp = 0.5, Ki = 0.1, Kd = 0.5;
int pulse = 0;
int pulseOverflow = 0;
bool home = true;
bool flag = false;

/* INITIALIZE PID TIMER, MOTOR PID MODEL, AND SENSOR */
hw_timer_t* pidTimer = NULL;
UltraSonicDistanceSensor distanceSensor(TRIGGERPIN, ECHOPIN); 
PID motorPID(&input, &output, &setPosition, Kp, Ki, Kd, DIRECT);

void setup()
{
  Serial.begin(9600);

  pinMode(MOTORPIN, OUTPUT);
  pinMode(IN1PIN, OUTPUT);
  pinMode(IN2PIN, OUTPUT);
  pinMode(SENSOR2PIN,INPUT);

  // LED states at 1Hz PWM and 16 bit resolution at channel 0
  ledcAttachPin(LEDPIN, LEDCHANNEL);
  ledcSetup(LEDCHANNEL, LEDFREQ, 16);

  // Motor PWM setup 30kHz at channel 1 and 2 at 8 bit resolution
  ledcAttachPin(IN1PIN, IN1CHANNEL);
  ledcSetup(IN1CHANNEL, MOTORFREQ, 8);
  ledcAttachPin(IN2PIN, IN2CHANNEL);
  ledcSetup(IN2CHANNEL, MOTORFREQ, 8);

  // ISRs
  pidTimer = timerBegin(2, 1, true); //pidTimer number 2, divider = 1 bc no prescalar needed
  timerAttachInterrupt(pidTimer, &PID, true);
  timerAlarmWrite(pidTimer, 80000 ,true); // Every 1Khz PID will run 80MHz/80000 = 1Khz
  motorPID.SetMode(AUTOMATIC);

  attachInterrupt(digitalPinToInterrupt(SENSOR2PIN), openClaw, RISING);
}

void loop()
{
  ledcWrite(LEDCHANNEL, SEVENTYPERC);
  digitalWrite(MOTORPIN, LOW);
  float distance = distanceSensor.measureDistanceCm();

  Serial.print("Distance from object: ");
  Serial.println(distance);
  
  if (distance <= 15.0 && distance >= 0&& home) {
    closeClaw();
  }
}

void closeClaw()
{
  Serial.println("Entering closeClaw state);
  ledcWrite(LEDCHANNEL, TENPERC);
  setPosition = 8544;
  flag = true;

  // Initialize motor pin without PWM
  digitalWrite(MOTORPIN, HIGH);
  ledcWrite(IN2PIN, 0);

  // Start PID
  home = false; 
  timerAlarmEnable(pidTimer);

  // Should not get to this case, but goes to next state if PID > 30s
  delay(30000);

  // Next state: Travel Claw
  travelClaw();
}

void PID() {
  if (!home) { 
    int pulseTemp = 0b0;
    bool pin;

    for (int i = 0; i < PLDCOMM; i++)
    {
      // PIN D0 - D9 (D0-D9)
      pulseTemp = pulseTemp << 1 | int(digitalRead(i));
    }

    Serial.print("Current pulse, prev pulse, overall pulse, max pulse: ");
    Serial.println(pulseTemp, pulseOverflow, pulse, MAXPULSE);

    //Motor running backwards
    // case that pulse count overflows backwards from 200 -> 15950
    // or motor goes backward 800 -> 400, chose 1000 as buffer as
    // pulses should not be greater than that with PID ISR
    if ((pulseTemp)*0.5 > pulseOverflow || (pulseOverflow-pulseTemp < 1000 && pulseOverflow-pulseTemp > 0)){
      pulse = pulse-(MAXPULSE + pulseOverflow - pulseTemp);
      pin = IN2PIN;
      Serial.print("Motor running backwards");
    } else {
    //Motor running forwards
    // case if pulse is regularly larger or is less than prev pulse
      pulse = pulse+(pulseTemp-pulseOverflow);
      pin = IN1PIN;
      Serial.print("Motor running forwards");
    }
    
    pulseOverflow = pulseTemp;
    input = pulse;
    motorPID.Compute();

    ledcWrite(pin, 255*abs((setPosition-output)/PRESETPOINT));

    Serial.print("New overall pulse, PID output: ");
    Serial.println(pulse, output);
    
    // PID settles close enough
    if (output <= setPosition+1 && output >= setPosition-1) {
      timerAlarmDisable(pidTimer);
      
       if (flag){
        travelClaw();
      } else {
        openClaw();
      }
    }
  }
}

void travelClaw()
{
  Serial.println("Entering travelClaw state);
  home = true;
  ledcWrite(LEDCHANNEL, FORTYPERC);
  
  // Motor stop spin, sanity check
  ledcWrite(IN1PIN, 0);
  ledcWrite(IN2PIN, 0);

  // Should not get to this case, but goes to next state if 
  // it doesn't see sensor for > 30s
  delay(30000);

  // Next state: openClaw()
  openClaw();
}

// Last state (ISR): 
void openClaw()
{
  Serial.println("Entering openClaw state);
  // ISR sense
  ledcWrite(LEDCHANNEL, ONEHUNPERC);
  setPosition = 0;
  flag = false;

  ledcWrite(IN1PIN, 0);

  // Start PID
  home = false;
  timerAlarmEnable(pidTimer);

  // Should not get to this case, but goes to next state if PID > 30s
  delay(30000);

  // Next state: Home
  home = true;
}
