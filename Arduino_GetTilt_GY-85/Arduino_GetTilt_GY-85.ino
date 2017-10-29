#include <ADXL345.h>
#include <Wire.h>
#include <ITG3200.h>

ADXL345 acc;
ITG3200 gyro = ITG3200(); // définition du type gyro
int time, dtime = 0;
float  gx, gy, gz;
int ax, ay, az;
float rawX, rawY, rawZ, deltime;
float angleGyroX, angleGyroY;
float angleAccX, angleAccY;
float angleFilterX, angleFilterY;

int jf = 46;
int jb = 48;
int jl = 50;
int jr = 52;
int rf = 47;
int rb = 49;
int rl = 51;
int rr = 53;

void setup() {
  pinMode(jf, OUTPUT);
  pinMode(jb, OUTPUT);
  pinMode(jl, OUTPUT);
  pinMode(jr, OUTPUT);
  pinMode(rf, OUTPUT);
  pinMode(rb, OUTPUT);
  pinMode(rl, OUTPUT);
  pinMode(rr, OUTPUT);
  
  Serial.begin(9600);
  delay(100);
  Serial.println("Starting...");
  acc.powerOn();
  Serial.println("Acc Init...");
  acc.setRangeSetting(8);
  Serial.println("Gyro Init...");
  gyro.init(ITG3200_ADDR_AD0_LOW);
  Serial.println("Gyro Calibrate...");
  gyro.zeroCalibrate(500, 1);
  Serial.println("Setup success !");
  delay(1000);
}

void loop() {
  time = millis();
  
  GetGyroAngle();
  GetAccAngle();

 
  Serial.println(angleGyroY);
  
  dtime = millis() - time;

  
}

void GetAccAngle(void)
{
  // calcul de l'angle en centième de degrés depuis l'attraction terrestre !
  acc.readAccel(&ax, &ay, &az); 
  rawX = ax + 37;
  rawY = ay + 62;
  rawZ = az - 326;
  angleAccX = -18000 * (atan(rawX / rawZ)) / PI;
  angleAccY = 18000 * (atan(rawY / rawZ)) / PI;
}


void GetGyroAngle(void) 
{
  // calcul de l'angle en centième de degrés depuis la vitesse angulaire
   gyro.readGyro(&gx,&gy,&gz); 
  angleGyroY += gx * dtime /10;
  angleGyroX += gy * dtime /10;
}




/*void LedDetect(void) {
  if(anglex >3) {
    digitalWrite(jf, LOW);
    digitalWrite(rf, LOW);
    digitalWrite(jb, HIGH);
    if(anglex >6) {digitalWrite(rb, HIGH);}
    else {digitalWrite(rb, LOW);}
  }
  else if(anglex <-3) {
    digitalWrite(jb, LOW);
    digitalWrite(rb, LOW);
    digitalWrite(jf, HIGH);
    if(anglex <-6) {digitalWrite(rf, HIGH);}
    else {digitalWrite(rf, LOW);}
  }
  else {
    digitalWrite(jb, LOW);
    digitalWrite(jf, LOW);
    digitalWrite(rb, LOW);
    digitalWrite(rf, LOW);
  }



  if(angley >3) {
    digitalWrite(jl, LOW);
    digitalWrite(rl, LOW);
    digitalWrite(jr, HIGH);
    if(angley >6) {digitalWrite(rr, HIGH);}
    else {digitalWrite(rr, LOW);}
  }
  else if(angley <-3) {
    digitalWrite(jr, LOW);
    digitalWrite(rr, LOW);
    digitalWrite(jl, HIGH);
    if(angley <-6) {digitalWrite(rl, HIGH);}
    else {digitalWrite(rl, LOW);}
  }
  else {
    digitalWrite(jr, LOW);
    digitalWrite(jl, LOW);
    digitalWrite(rr, LOW);
    digitalWrite(rl, LOW);
  }

  
}*/

