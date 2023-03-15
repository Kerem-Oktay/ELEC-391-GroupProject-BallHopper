volatile long countData = 0;


void setup() {
  Serial.begin(9600);

  pinMode(13, OUTPUT);
  digitalWrite(13, LOW); 
  delay(10);
  digitalWrite(13, HIGH); // Keeps the reset off
  

  for(int i = 2; i<10; i++) {         //Puts pins 2 to 9 to input
    pinMode(i, INPUT);
  }

  for(int j = 10; j<13; j++) {     // Puts pins 10 to 12 as outputs and sets them initially to low
    pinMode(j, OUTPUT);
    digitalWrite(j, LOW);
    
  }

}

byte getMSB(){

byte MSBold = 0; // Initialize a byte variable to store the position data

for (int i = 0; i < 8; i++) {
  MSBold |= (digitalRead(2+i) << i); // Read each data pin and set the corresponding bit in the byte variable
}
byte MSBnew = 0;

for (int i = 0; i < 8; i++) {
  MSBnew |= (digitalRead(2+i) << i); // Read each data pin and set the corresponding bit in the byte variable
}

  if (MSBnew == MSBold){ 
    byte MSBresult = MSBnew;
    return MSBresult;  
  }
  else getMSB();
}

byte getSecond(){

byte secondOld = 0; // Initialize a byte variable to store the position data

for (int i = 0; i < 8; i++) {
  secondOld |= (digitalRead(2+i) << i); // Read each data pin and set the corresponding bit in the byte variable
}
byte secondNew = 0;

for (int i = 0; i < 8; i++) {
  secondNew |= (digitalRead(2+i) << i); // Read each data pin and set the corresponding bit in the byte variable
}
  if (secondNew == secondOld){ 
    byte secondResult = secondNew;
    return secondResult;  
  }
  else getSecond();
}

byte getThird(){

byte thirdOld = 0; // Initialize a byte variable to store the position data

for (int i = 0; i < 8; i++) {
  thirdOld |= (digitalRead(2+i) << i); // Read each data pin and set the corresponding bit in the byte variable
}
byte thirdNew = 0;

for (int i = 0; i < 8; i++) {
  thirdNew |= (digitalRead(2+i) << i); // Read each data pin and set the corresponding bit in the byte variable
}
  if (thirdNew == thirdOld){ 
    byte thirdResult = thirdNew;
    return thirdResult;  
  }
  else getThird();
}

byte getLSB(){
byte LSBold = 0; // Initialize a byte variable to store the position data

for (int i = 0; i < 8; i++) {
  LSBold |= (digitalRead(2+i) << i); // Read each data pin and set the corresponding bit in the byte variable
}
byte LSBnew = 0;

for (int i = 0; i < 8; i++) {
  LSBnew |= (digitalRead(2+i) << i); // Read each data pin and set the corresponding bit in the byte variable
}
  if (LSBnew == LSBold){  
    byte LSBresult = LSBnew;
    return LSBresult;  
  }
  else getLSB();
}

long mergeFunc(byte MSBresult, byte secondResult, byte thirdResult, byte LSBresult){
  long tempVar = 0;
  tempVar |= ((long) MSBresult << 24) | ((long) secondResult << 16) | ((long) thirdResult << 8) | ((long) LSBresult << 0);
  countData = tempVar;
  return countData;
}



void loop() {
  delay(25);
  digitalWrite(12, HIGH); // Diable OE
  digitalWrite(12, LOW); // Enable OE
  digitalWrite(10, LOW);
  digitalWrite(11, HIGH); // SEL1 = 0 and SEL2 = 1
  
 
  byte MSBresult = getMSB();

  
  digitalWrite(10, HIGH);
  digitalWrite(11, HIGH); // SEL1 = 1 and SEL2 = 1
  byte secondResult = getSecond();
  
  digitalWrite(10, LOW);
  digitalWrite(11, LOW); // SEL1 = 0 and SEL2 = 0
  byte thirdResult = getThird();
  
  digitalWrite(10, HIGH);
  digitalWrite(11, LOW); // SEL1 = 1 and SEL2 = 0
  byte LSBresult = getLSB();

  digitalWrite(12, HIGH); // disable OE
  delay(25);
  countData = mergeFunc(MSBresult, secondResult, thirdResult, LSBresult);
  float angle = (countData % 64)*(360.0/64.0) ;
  Serial.print("Counter: ");
  Serial.println(countData);
  Serial.println("\n");
  Serial.print("Angle: ");
  Serial.println(angle);



  }
