/*
 * Pinout MEGA : 
 * Radio : 
 * 53 CE
 * 48 CSN
 * SPI on ICSP header (3 pins)
 * 
 * serv1 : Droite, pin 3
 * serv2 : Gauche, pin 4
 * esc1  : Droite, pin 5
 * esc2  : Gauche, pin 6
 */

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
float rawX, rawY, rawZ, dtime;
float angleAccX, angleAccY;
float histAccY[50];
float avgAccY;
float  gx, gy, gz;
float angleGyroX, angleGyroY, angleGyroZ;
float tima=0;
float compX, compY, compZ;
int Pitch, Roll;

int histT1[500]={0};
int avgT1 = 0;
int histT2[500]={0};
int avgT2 = 0;
int servNew, servOld = 1470;


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

int erreur = 0;
float somme_erreurs = 0;
float variation_erreur = 0;
int erreur_precedente = 0;
int commande = 1470;
float Kp = 0.002;
float Ki = 0;//.00005;
float Kd = 0;//0.005;

int target1 = 0;
int offsetPitch = 0;
int offsetYaw = 0;
int offsetRoll = 0;
byte addrOffsetPitch = 0;
byte addrOffsetYaw = 2;
byte addrOffsetRoll = 4;
int eepromTime = 0;

int ESC1Value = 0;
int ESC2Value = 0;

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
  Serial.println("Accelerometer started !");
  delay(100);
  Serial.println("Starting...");
  Serial.println("Gyro Init...");
  gyro.init(ITG3200_ADDR_AD0_LOW);
  Serial.println("Gyro Calibrate...");
  gyro.zeroCalibrate(500, 1);
  Serial.println("Gyro started !");
  
  servo1.attach(3);
  servo2.attach(4);
  GetAllTilt();
  servo1.write(90);
  servo2.write(90);

  

  offsetPitch = EEPROM.read(addrOffsetPitch);
  offsetYaw = EEPROM.read(addrOffsetYaw);
  offsetRoll = EEPROM.read(addrOffsetRoll);

  delay(200);

  ESC1.attach(5);
  ESC2.attach(6);
  ESC1.write(0);
  ESC2.write(0);

  Serial.println("Setup success !");
  delay(700);
}

void loop()
{
  getRadio();
  delay(10); 


  ESC1Value = map(Y2, 50, 100, 1000, 2000);
  if(deadTimes > 10)
  {
    ESC1Value = 100;
    ESC2Value = 100;
  }
  ESC1.writeMicroseconds(ESC1Value);
  ESC2.writeMicroseconds(ESC1Value);
  //ESC1.writeMicroseconds(900);
  //ESC2.writeMicroseconds(900);


  EEPROMOffsetManage();


  //target1 = ((40*(-50+Y1))+20*T1-20*T2)+5*offsetPitch-400;
  target1 = ((40*(-50+Y1))+20*T1-20*T2);

  
  GetAllTilt();
  //Serial.println(Pitch);
  serv1new(target1); //target pitch
  //Serial.print(Y1);
  //Serial.print(",  ");
  //Serial.println(servNew);
  //print360();
  serv2();
  //Serial.println(avgPitch);
}

////////////////////////////////////////////////////             GetAllTilt             /////////////////////////////////////////////////////////


void GetAllTilt(void) {
  GetAccTilt();
  GetGyroAngle();
  
  compX = 0.99*(compX)+0.01*(-avgAccY/100);
  compY = 0.99*(compY)+0.01*(-angleAccX/100);

  Pitch = -compX * 100;
  Roll = compY * 100;
}

////////////////////////////////////////////////////             GetGyroAngle             /////////////////////////////////////////////////////////


void GetGyroAngle(void) {
  // calcul de l'angle depuis la vitesse angulaire
   gyro.readGyro(&gx,&gy,&gz); 
   dtime = millis() - tima;
  compX += gx * dtime /1000;
  compY += gy * dtime /1000;
  angleGyroZ += gz * dtime /1000;
   tima = millis();
   Serial.println(gx * dtime /1000);
}


////////////////////////////////////////////////////             serv1new             /////////////////////////////////////////////////////////

void serv1new (int target)
{
 
  erreur = target - Pitch;
  //Serial.println("                    erreur = target - Pitch");
  /*Serial.print("                    ");
  Serial.print(erreur);
  Serial.print(" = ");
  Serial.print(target);
  Serial.print(" - ");
  Serial.println(Pitch);*/
  somme_erreurs += erreur;
  variation_erreur = erreur - erreur_precedente;
  commande = 0.01 * erreur;
  erreur_precedente = erreur;

  servNew = servOld-commande;

  if(servNew<=600)                        { servNew = 600; }                      //610
  else if(servNew>=2300)                  { servNew = 2300; }                     //2330
  servo1.writeMicroseconds(servNew);

 /*  Serial.print("                                                     ");
  Serial.println(servNew);*/
  
  servOld = servNew;

  //Serial.println(servNew);
}

////////////////////////////////////////////////////             serv2             /////////////////////////////////////////////////////////

void serv2()
{
  int servNewDeux = map(((1470-(servNew-1470))-5*avgT1+5*avgT2+1.3*offsetYaw-130), 1050, 1600, 1040, 1440);
  if(servNewDeux<=600)                        { servNewDeux = 600; }                      //610
  else if(servNewDeux>=2300)                  { servNewDeux = 2300; }                     //2330
  
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

  //Pitch = angleAccY;
  averageAccY();
  //roll = angleAccX;
}


////////////////////////////////////////////////////             averageAccY             /////////////////////////////////////////////////////////


void averageAccY(void)
{
  int i;
  long tempy;
  tempy=0;
  for(i=49; i>0; i--)      { histAccY[i] = histAccY[i-1]; }
  histAccY[0] = angleAccY;
  for(i=49; i>=0; i--)     { tempy = (tempy + histAccY[i]); }
  avgAccY = tempy/50;
  //Serial.print("              ");
  //Serial.println(avgAccY);
}


////////////////////////////////////////////////////             averageT1             /////////////////////////////////////////////////////////

void averageT1(void)
{
  int i;
  for(i=499; i>0; i--)      { histT1[i] = histT1[i-1]; }
  histT1[0] = T1;
  for(i=499; i>=0; i--)     { avgT1 = (avgT1 + histT1[i]) / 2; }
}

////////////////////////////////////////////////////             averageT2             /////////////////////////////////////////////////////////

void averageT2(void)
{
  int i;
  for(i=499; i>0; i--)      { histT2[i] = histT2[i-1]; }
  histT2[0] = T2;
  for(i=499; i>=0; i--)     { avgT2 = (avgT2 + histT2[i]) / 2; }
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

////////////////////////////////////////////////////             EEPROMOffsetManage             /////////////////////////////////////////////////////////


void EEPROMOffsetManage (void) {
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
