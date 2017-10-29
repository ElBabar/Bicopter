#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(53, 48);

const byte rxAddr[6] = "00001";
const byte buttonBase = 0;
byte button;

 int pinX1 = 0;
 int pinY1 = 1;
 int pinX2 = 2;
 int pinY2 = 3;
 int pinT1 = 4;
 int pinT2 = 5;
 int pinA = 30;
 int pinB = 32;
 int pinX = 34;
 int pinY = 36;
 int pinSt = 44;
 int pinBa = 46;

 int X1;
 int Y1;
 int X2;
 int Y2;
 int T1;
 int T2;
 int A;
 int B;
 int X;
 int Y;
 int St;
 int Ba;
 
 int X1perCent = 50;
 int Y1perCent = 50;
 int X2perCent = 50;
 int Y2perCent = 50;
 int T1perCent = 0;
 int T2perCent = 0;

byte buttonA;
byte buttonB;
byte buttonX;
byte buttonY;
byte buttonBa;
byte buttonSt;
 
void setup() {
  Serial.begin(115200);
  radio.begin();
  radio.setRetries(15, 15);
  radio.openWritingPipe(rxAddr);
  radio.stopListening();
}

void loop() {
  input();

  printperCent();  

  buttonA = A << 5;
  buttonB = B << 4;
  buttonX = X << 3;
  buttonY = Y << 2;
  buttonBa = Ba << 1;
  buttonSt = St;
  button = buttonBase | buttonA | buttonB | buttonX | buttonY | buttonBa | buttonSt;
  
  char text2[7] = {char(X1perCent), char(Y1perCent), char(X2perCent), char(Y2perCent), char(T1perCent), char(T2perCent), char(button)};
  radio.write(&text2, sizeof(text2));
  //Serial.println(X1);

}

void input (void)
{
  X1 = analogRead(pinX1);
  Y1 = analogRead(pinY1);
  X2 = analogRead(pinX2);
  Y2 = analogRead(pinY2);
  T1 = analogRead(pinT1);
  T2 = analogRead(pinT2);

  A = !digitalRead(pinA);
  B = !digitalRead(pinB);
  X = !digitalRead(pinX);
  Y = !digitalRead(pinY);
  St = !digitalRead(pinSt);
  Ba = !digitalRead(pinBa);
  
  X1perCent = 100 - axisPerCent(X1, 173, 775, 495, 13);
  Y1perCent = axisPerCent(Y1, 212, 800, 515, 10);  
  X2perCent = 100 - axisPerCent(X2, 185, 615, 510, 16);
  Y2perCent = axisPerCent(Y2, 245, 610, 523, 15); 
  T1perCent = axisPerCent(T1, 240, 800, 550, 0); 
  T2perCent = axisPerCent(T2, 230, 855, 550, 0); 
  
}



void printRawInput (void)
{
  Serial.print("X1: ");
  Serial.print(X1);
  Serial.print("   Y1: ");
  Serial.print(Y1);
  Serial.print("   X2: ");
  Serial.print(X2);
  Serial.print("   Y2: ");
  Serial.print(Y2);
  Serial.print("   T1: ");
  Serial.print(T1);
  Serial.print("   T2: ");
  Serial.print(T2);
  Serial.print("   A: ");
  Serial.print(A);
  Serial.print("   B: ");
  Serial.print(B);
  Serial.print("   X: ");
  Serial.print(X);
  Serial.print("   Y: ");
  Serial.print(Y);
  Serial.print("   St: ");
  Serial.print(St);
  Serial.print("   Ba: ");
  Serial.println(Ba);
  
}


void printperCent(void)
{
  Serial.print(X1perCent);
  Serial.print("  ,  ");
  Serial.print(Y1perCent);
  Serial.print("  |  ");
  Serial.print(X2perCent);
  Serial.print("  ,  ");
  Serial.print(Y2perCent);
  Serial.print("         |         ");
  Serial.print(T1perCent);
  Serial.print("  |  ");
  Serial.println(T2perCent);
}

int axisPerCent(int Axis, int AxisMin, int AxisMax, int AxisMiddle, int AxisdeadZone)
{
  int AxisperCent;
  
  if (Axis>=(AxisMiddle + 3*AxisdeadZone)) {
  AxisperCent = constrain((map(Axis, (AxisMiddle + 3*AxisdeadZone), AxisMax, 50, 100)), 50, 100);}
  else if (Axis<(AxisMiddle - 3*AxisdeadZone)) {
AxisperCent = constrain((map(Axis, AxisMin, (AxisMiddle - 3*AxisdeadZone), 0, 50)), 0, 50);}
  else {
  AxisperCent = 50;}

  return AxisperCent;
}









