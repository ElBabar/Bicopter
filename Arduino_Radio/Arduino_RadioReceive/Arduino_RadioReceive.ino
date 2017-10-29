#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>


RF24 radio(53, 48);

const byte rxAddr[6] = "00001";

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
  if (radio.available())
  {
    Serial.println("receiving. . . ");
    char text[32] = {0};
    radio.read(&text, sizeof(text));
    
    Serial.println(text);
    
  }
  else {Serial.println("No radio");}
  delay(10);
}



