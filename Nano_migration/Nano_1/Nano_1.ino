#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>
#include <Wire.h>
#include <ADXL345.h>
#include <Servo.h>
#include <ITG3200.h>
#include <EEPROM.h>


ADXL345 acc;
ITG3200 gyro = ITG3200();
Servo servo1;
Servo servo2;
Servo ESC1;
Servo ESC2;



RF24 radio(53, 48);
const byte rxAddr[6] = "00001";

int ax, ay, az;
float  gx, gy, gz;
float histAngularX[50]={0};
float avgAngularX = 0;
float histAngularY[50]={0};
float avgAngularY = 0;
float rawX, rawY, rawZ;
int angleAccX, angleAccY;
int histPitch[200]={1470};
int avgPitch = 1470;
int histT1[200]={0};
int avgT1 = 0;
int histT2[200]={0};
int avgT2 = 0;
int Pitch;
int servNew, servOld = 1470;
int dtime=0, time=0, i, j, k, l;


int X1 = 0;
int Y1 = 0;
int X2 = 0;
int Y2 = 0;
int T1 = 0;
int T2 = 0;
int A = 0;
int B = 0;
int X = 0;
int Y = 0;
int Ba = 0;
int St = 0;
int deadTimes = 0;

float erreur = 0;
float somme_erreurs = 0;
float variation_erreur = 0;
float erreur_precedente = 0;
float commande = 1470;
float Kp = 0.002;
float Ki = 0;//.00005;
float Kd = 0;//0.005;

int target1 = 0;
byte offsetPitch = 0;
byte offsetYaw = 0;
byte offsetRoll = 0;
byte addrOffsetPitch = 0;
byte addrOffsetYaw = 2;
byte addrOffsetRoll = 4;
int eepromTime = 0;

int ESC1Value = 1500;
int ESC2Value = 1500;

void setup()
{
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);
  
  Serial.begin(115200);

  radio.begin();
  radio.openReadingPipe(0, rxAddr);
  radio.startListening();
  
  acc.powerOn();
  acc.setRangeSetting(8);

  gyro.init(ITG3200_ADDR_AD0_LOW);
  gyro.zeroCalibrate(500, 1);
  
  servo1.attach(3);
  servo2.attach(4);
  GetAccTilt();
  Pitch = angleAccY;
  servo1.write(90);
  servo2.write(90);

  

  offsetPitch = EEPROM.read(addrOffsetPitch);
  offsetYaw = EEPROM.read(addrOffsetYaw);
  offsetRoll = EEPROM.read(addrOffsetRoll);

  delay(200);

  ESC1.attach(5);
  ESC2.attach(6);
}

void loop()
{
  //dtime = millis()-time;
  //time = millis();
  getRadio();
  delay(10); 

  ESC1Value = map(Y2, 50, 100, 1000, 2000);
  //Serial.println(avgPitch);
  //Serial.println(offsetPitch);

  if(deadTimes > 10)
  {
    ESC1Value = 1000;
    ESC2Value = 1000;
  }

  ESC1.writeMicroseconds(ESC1Value);
  ESC2.writeMicroseconds(ESC1Value);

  eepromTime += 1;
  if (eepromTime == 500)
  {
    digitalWrite(13, HIGH);
    EEPROM.update(addrOffsetPitch, offsetPitch);
    delay(2);
    digitalWrite(13, LOW);
  }
  else if (eepromTime == 1000)
  {
    digitalWrite(13, HIGH);
    EEPROM.update(addrOffsetYaw, offsetYaw);
    delay(2);
    digitalWrite(13, LOW);
  }
  else if (eepromTime > 1500)
  {
    digitalWrite(13, HIGH);
    eepromTime = 0;
    EEPROM.update(addrOffsetRoll, offsetRoll);
    delay(2);
    digitalWrite(13, LOW);
  }
  if (Y == 1) {offsetPitch += 1;}
  if (A == 1) {offsetPitch -= 1;}
  if (B == 1) {offsetPitch = 0;}
  if (St == 1) {offsetYaw += 1;}
  if (Ba == 1) {offsetYaw -= 1;}
  //Serial.println(offsetYaw);
  

  target1 = ((40*(-50+Y1))+20*T1-20*T2)+5*offsetPitch-400;
  
  GetAccTilt();
  serv1new(target1); //target pitch
  //Serial.print(Y1);
  //Serial.print(",  ");
  //Serial.println(servNew);
  //print360();
  serv2();
  //Serial.println(avgPitch);
}

////////////////////////////////////////////////////             serv1new             /////////////////////////////////////////////////////////

void serv1new (int target)
{
 
  erreur = target - avgPitch;
  somme_erreurs += erreur;
  variation_erreur = erreur - erreur_precedente;
  commande = Kp * erreur * (0.000*abs(erreur)+1) + Ki * somme_erreurs + Kd * variation_erreur;
  erreur_precedente = erreur;

  servNew = servOld-commande;

  if(servNew<=1000)                        { servNew = 1000; }                      //610
  else if(servNew>=1900)                  { servNew = 1900; }                     //2330
  servo1.writeMicroseconds(servNew);
  
  servOld = servNew;

  //Serial.println(servNew);
}

////////////////////////////////////////////////////             serv2             /////////////////////////////////////////////////////////

void serv2()
{
  int servNewDeux = map(((1470-(servNew-1470))-5*avgT1+5*avgT2+1.3*offsetYaw-130), 1050, 1600, 1040, 1440);
  if(servNewDeux<=1000)                        { servNewDeux = 1000; }                      //610
  else if(servNewDeux>=1900)                  { servNewDeux = 1900; }                     //2330
  
  servo2.writeMicroseconds(servNewDeux);  
  //Serial.println(servNewDeux);
}

////////////////////////////////////////////////////             GetAccTilt             /////////////////////////////////////////////////////////

void GetAccTilt(void)
{
  acc.readAccel(&ax, &ay, &az);
  rawX = ax + 37;
  rawY = ay + 62;
  rawZ = az - 326;
  
  if(rawZ >= 0)               { angleAccX = 100*(-180 * (atan(rawX / rawZ)) / PI); }
  if((rawZ < 0) && (rawX>=0)) { angleAccX = 100*(-180 + (-180 * (atan(rawX / rawZ)) / PI)); }
  if((rawZ < 0) && (rawX<0))  { angleAccX = 100*(180 + (-180 * (atan(rawX / rawZ)) / PI)); }

  if(rawZ >= 0)               { angleAccY = 100*(-180 * (atan(rawY / rawZ)) / PI); }
  if((rawZ < 0) && (rawY>=0)) { angleAccY = 100*(-180 + (-180 * (atan(rawY / rawZ)) / PI)); }
  if((rawZ < 0) && (rawY<0))  { angleAccY = 100*(180 + (-180 * (atan(rawY / rawZ)) / PI)); }

  Pitch = angleAccY;
  averagePitch();
  //roll = angleAccX;
}

////////////////////////////////////////////////////             averagePitch             /////////////////////////////////////////////////////////

void averagePitch(void)
{
  int i;
  for(i=199; i>0; i--)      { histPitch[i] = histPitch[i-1]; }
  if (Pitch<avgPitch) {Pitch = avgPitch - 200;}
  else {Pitch = avgPitch + 200;}
  histPitch[0] = Pitch;
  for(i=199; i>=0; i--)     { avgPitch = (avgPitch + histPitch[i]) / 2; }
}

////////////////////////////////////////////////////             averageT1             /////////////////////////////////////////////////////////

void averageT1(void)
{
  int i;
  for(i=199; i>0; i--)      { histT1[i] = histT1[i-1]; }
  histT1[0] = T1;
  for(i=199; i>=0; i--)     { avgT1 = (avgT1 + histT1[i]) / 2; }
}

////////////////////////////////////////////////////             averageT2             /////////////////////////////////////////////////////////

void averageT2(void)
{
  int i;
  for(i=199; i>0; i--)      { histT2[i] = histT2[i-1]; }
  histT2[0] = T2;
  for(i=199; i>=0; i--)     { avgT2 = (avgT2 + histT2[i]) / 2; }
}

////////////////////////////////////////////////////             print 360             /////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////             getRadio             /////////////////////////////////////////////////////////

void getRadio(void)
{
  if (radio.available())
  {
    char text[7] = {0};
    radio.read(&text, sizeof(text));

    if ((text[0] == 0) && (text[1] == 0) && (text[2] == 0) && (text[3] == 0) && (text[4] == 0) && (text[5] == 0) && (text[6] == 0))  
    {
      if (deadTimes <= 10){Serial.println("Radio missed");}
      if ((deadTimes >= 10) && (deadTimes % 50 == 0)){Serial.println("Radio is Dead");}
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

      //print360();
      deadTimes = 0;
    }
  }

  
  else 
  {
    deadTimes += 1;
    if ((deadTimes >= 10) && (deadTimes % 50 == 0)){Serial.println("Radio is Dead");}
    else if (deadTimes <= 10){Serial.println("No radio");}
  }

  
  averageT1();
  averageT2();
  
}

////////////////////////////////////////////////////             averageAngular             /////////////////////////////////////////////////////////

void averageAngularX(void)
{
  int i;
  for(i=49; i>0; i--)      { histAngularX[i] = histAngularX[i-1]; }
  histAngularX[0] = abs(gx);
  for(i=49; i>=0; i--)     { avgAngularX = (avgAngularX + histAngularX[i]) / 2; }
}

void averageAngularY(void)
{
  int i;
  for(i=49; i>0; i--)      { histAngularY[i] = histAngularY[i-1]; }
  histAngularY[0] = abs(gy);
  for(i=49; i>=0; i--)     { avgAngularY = (avgAngularY + histAngularY[i]) / 2; }
}




////////////////////////////////////////////////////             serv1             /////////////////////////////////////////////////////////
/*
void serv1 (int target)
{
  int relMove;
  int diff = (target*100-avgPitch)/2;
  int noDiff = map(abs(diff), 0, 90000, 28, 1);                  // l'avant dernière règle le gain. faible = rapide et instable. A besoin d'être changée si dtime évolue.
  relMove = abs(diff/noDiff);
  
  servNew = servOld-diff;

  if((servNew-servOld)<relMove)           { servNew = servOld - relMove; }
  else if((servNew-servOld)>=relMove)     { servNew = servOld + relMove; }
  if(servNew<=610)                        { servNew = 610; }
  else if(servNew>=2330)                  { servNew = 2330; }
  servo1.writeMicroseconds(servNew);
  
  servOld = servNew;
}*/
