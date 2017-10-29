#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(53, 48);

const byte rxAddr[6] = "00001";

int X1 = 10;
int Y1 = 20;
int X2 = 30;
int Y2 = 40;
int T1 = 50;
int T2 = 60;

int A = 1;
byte buttonA;
int B = 0;
byte buttonB;
int X = 0;
byte buttonX;
int Y = 1;
byte buttonY;
int Ba = 0;
byte buttonBa;
int St = 1;
byte buttonSt;

const byte buttonBase = 0;
byte button;

void setup()
{
  Serial.begin(115200);
  radio.begin();
  radio.setRetries(15, 15);
  radio.openWritingPipe(rxAddr);
  radio.stopListening();
}

void loop()
{
  buttonA = A << 5;
  buttonB = B << 4;
  buttonX = X << 3;
  buttonY = Y << 2;
  buttonBa = Ba << 1;
  buttonSt = St;

  button = buttonBase | buttonA | buttonB | buttonX | buttonY | buttonBa | buttonSt;
  
  char text2[7] = {char(X1), char(Y1), char(X2), char(Y2), char(T1), char(T2), char(button)};
  
  //Serial.println(text2);
  radio.write(&text2, sizeof(text2));
}





