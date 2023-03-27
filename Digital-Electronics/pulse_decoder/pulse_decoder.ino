qwerqwerqwer
// Control Signals
int RSTN = 2;
int OEN  = 3;
int SEL1 = 4;
int SEL2 = 5;

// Counter Signals
int D0 = 6;
int D1 = 7;
int D2 = 8;
int D3 = 9;
int D4 = 10;
int D5 = 11;
int D6 = 12;
int D7 = 13;

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

  // Assign Input Pins
  pinMode(D0,INPUT);
  pinMode(D1,INPUT);
  pinMode(D2,INPUT);
  pinMode(D3,INPUT);
  pinMode(D4,INPUT);
  pinMode(D5,INPUT);
  pinMode(D6,INPUT);
  pinMode(D7,INPUT);

  // Initial Decoder Reset
  digitalWrite(RSTN,LOW);
  delay(15);
  digitalWrite(RSTN,HIGH);

  // Initial Control Signals
  digitalWrite(OEN,LOW);
  digitalWrite(SEL1,LOW);
  digitalWrite(SEL2,LOW);

  // Serial Port for Debug Purposes
  Serial.begin(9600);
}

byte getByte()
{
byte capture1 = 0; // Initialize a byte variable to read the first capture
for (int i = 0; i < 8; i++) {
  capture1 |= (digitalRead(D0+i) << i); // Read Input Pins for the first capture
}

byte capture2 = 0; // Initialize a byte variable to read the second capture
for (int i = 0; i < 8; i++) {
  capture2 |= (digitalRead(D0+i) << i); // Read Input Pins for the second capture
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
  digitalWrite(OEN, LOW); // Enable OE

  getPulseCount();

  float angle = (count % 64)*(360.0/64.0) ;
  float angle_abs = (count)*(360.0/64.0) ;

  digitalWrite(OEN, HIGH); // Disable OE
  Serial.print("Pulse Count: ");
  Serial.println(count);
  Serial.print("Angle: ");
  Serial.println(angle);
  Serial.println("\n");
  
}
