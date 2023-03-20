// Control Signals
int RSTN = 2;
int OEN  = 3;
int SEL1 = 4;
int SEL2 = 5;
int S1   = 6;
int S2   = 7;

// Counter Signals
int BIT0 = 8;
int BIT1 = 9;

volatile long count = 0;

/*
  The algorithm below is based on the Example Routine given in the IC datasheet
*/

void setup() 
{
  // Assign Output Pins
  pinMode(RSTN,OUTPUT);
  pinMode(OEN,OUTPUT);
  pinMode(SEL1,OUTPUT);
  pinMode(SEL2,OUTPUT);
  pinMode(S1,OUTPUT);
  pinMode(S2,OUTPUT);

  // Assign Input Pins
  pinMode(BIT0,INPUT);
  pinMode(BIT1,INPUT);

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

  // Serial Port for Debug Purposes
  Serial.begin(9600);
}

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

void loop() {
  // put your main code here, to run repeatedly:
  delay(25);
  digitalWrite(OEN, HIGH); // Disable OE

  getPulseCount();

  float angle = (count % 64)*(360.0/64.0) ;
  float angle_abs = (count)*(360.0/64.0) ;

  Serial.print("Counter: ");
  Serial.println(count);

  Serial.print("Angle: ");
  Serial.println(angle);

  Serial.print("Absolute Angle: ");
  Serial.println(angle_abs);
    
  Serial.println("\n");
}