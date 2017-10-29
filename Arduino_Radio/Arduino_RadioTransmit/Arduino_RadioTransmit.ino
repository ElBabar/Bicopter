#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(53, 48);

const byte rxAddr[6] = "00001";

void setup()
{
  radio.begin();
  radio.setRetries(15, 15);
  radio.openWritingPipe(rxAddr);
  
  radio.stopListening();

  pinMode(13, OUTPUT);
}

void loop()
{
  //digitalWrite(13, LOW);
  const char text[] = "Hello Bitch";
  radio.write(&text, sizeof(text));
  
  //delay(10);
  //digitalWrite(13, HIGH);
  //delay(10);
  
}
