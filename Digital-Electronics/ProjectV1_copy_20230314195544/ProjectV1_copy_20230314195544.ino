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

void setup() {
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

byte getByte(){

byte byteOld = 0; // Initialize a byte variable to store the position data

for (int i = 0; i < 8; i++) {
  byteOld |= (digitalRead(D0+i) << i); // Read each data pin and set the corresponding bit in the byte variable
}
byte byteNew = 0;

for (int i = 0; i < 8; i++) {
  byteNew |= (digitalRead(D0+i) << i); // Read each data pin and set the corresponding bit in the byte variable
}
  if (byteNew == byteOld){ 
    byte byteResult = byteNew;
    return byteResult;  
  }
  else getByte();
}

long mergeFunc(byte MSBresult, byte secondByte, byte thirdByte, byte LSBresult){
  long tempVar = 0;
  tempVar |= ((long) MSBresult << 24) | ((long) secondByte << 16) | ((long) thirdByte << 8) | ((long) LSBresult << 0);
  countData = tempVar;
  return countData;
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(25);
  digitalWrite(12, HIGH); // Diable OE
  digitalWrite(12, LOW); // Enable OE
  digitalWrite(10, LOW);
  digitalWrite(11, HIGH); // SEL1 = 0 and SEL2 = 1
  
  byte MSBresult = getByte();

  digitalWrite(10, HIGH);
  digitalWrite(11, HIGH); // SEL1 = 1 and SEL2 = 1
  byte secondResult = getByte();
  
  digitalWrite(10, LOW);
  digitalWrite(11, LOW); // SEL1 = 0 and SEL2 = 0
  byte thirdResult = getByte();
  
  digitalWrite(10, HIGH);
  digitalWrite(11, LOW); // SEL1 = 1 and SEL2 = 0
  byte LSBresult = getByte();

  digitalWrite(12, HIGH); // disable OE
  delay(25);
  countData = mergeFunc(MSBresult, secondByte, thirdByte, LSBresult);
  float angle = (countData % 64)*(360.0/64.0) ;
  Serial.print("Counter: ");
  Serial.println(countData);
  Serial.println("\n");
  Serial.print("Angle: ");
  Serial.println(angle);
}
