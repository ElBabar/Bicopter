#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>


RF24 radio(53, 48);

const byte rxAddr[6] = "00001";

int X1 = 0;
int Y1 = 0;
int X2 = 0;
int Y2 = 0;
int T1 = 0;
int T2 = 0;

int A;
int B;
int X;
int Y;
int Ba;
int St;

int deadTimes = 0;

void setup()
{
  while (!Serial);
  Serial.begin(115200);
  
  radio.begin();
  radio.openReadingPipe(0, rxAddr);
  
  radio.startListening();
}

void loop()
{
  getRadio();
  delay(10);
  
}


void print360(void)
{
  Serial.print(X1);
  Serial.print("  ,  ");
  Serial.print(Y1);
  Serial.print("  |  ");
  Serial.print(X2);
  Serial.print("  ,  ");
  Serial.print(Y2);
  Serial.print("  |  ");
  Serial.print(T1);
  Serial.print("  ,  ");
  Serial.print(T2);

  Serial.print("       ---       ");
  Serial.print(A);
  Serial.print("     ");
  Serial.print(B);
  Serial.print("     ");
  Serial.print(X);
  Serial.print("     ");
  Serial.print(Y);
  Serial.print("     ");
  Serial.print(Ba);
  Serial.print("     ");
  Serial.println(St);
  
}

void getRadio(void)
{
  if (radio.available())
  {
    char text[7] = {0};
    radio.read(&text, sizeof(text));

    if ((text[0] == 0) && (text[1] == 0) && (text[2] == 0) && (text[3] == 0) && (text[4] == 0) && (text[5] == 0) && (text[6] == 0))  
    {
      if (deadTimes <= 10)
      {
        Serial.println("Radio missed");
      }
      if ((deadTimes >= 10) && (deadTimes % 50 == 0))
      {
        Serial.println("Radio is Dead");
      }
      deadTimes += 1;
    }
    
    else 
    {
      X1 = text[0];
      Y1 = text[1];
      X2 = text[2];
      Y2 = text[3];
      T1 = text[4];
      T2 = text[5];

      St = text[6] & 1;
      Ba = (text[6] & 2) >> 1;
      Y = (text[6] & 4) >> 2;
      X = (text[6] & 8) >> 3;
      B = (text[6] & 16) >> 4;
      A = (text[6] & 32) >> 5;

      print360();

      deadTimes = 0;
    }
  }

  
  else 
  {
    deadTimes += 1;
    if ((deadTimes >= 10) && (deadTimes % 50 == 0))
    {
      Serial.println("Radio is Dead");
    }
    else if (deadTimes <= 10)
    {
      Serial.println("No radio");
    }
  }

  
}













