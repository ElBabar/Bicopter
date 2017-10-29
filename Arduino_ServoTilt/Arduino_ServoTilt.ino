#include <Wire.h>
#include <ADXL345.h>
#include <Servo.h>

ADXL345 acc;
Servo servo1;
int ax, ay, az;
float rawX, rawY, rawZ;
int angleAccX, angleAccY;
int histPitch[50]={1470};
int avgPitch = 1470;
int Pitch;
int servNew, servOld = 1470;
int dtime=0, time=0, i, j, k, l;

void setup()
{
  Serial.begin(9600);
  acc.powerOn();
  acc.setRangeSetting(8);
  servo1.attach(9);
  GetAccTilt();
  Pitch = angleAccY;
  servo1.write(90);
}

void loop()
{
  //dtime = millis()-time;
  //time = millis();
  GetAccTilt();

  serv1(0); //target pitch
  //Serial.println(Pitch);
}

////////////////////////////////////////////////////             serv1             /////////////////////////////////////////////////////////

void serv1 (int target)
{
  int relMove;
  int diff = (target*100-avgPitch)/2;
  int noDiff = map(abs(diff), 0, 90000, 100, 1);                  // l'avant dernière règle le gain. faible = rapide et instable. A besoin d'être changée si dtime évolue.
  relMove = abs(diff/noDiff);
  
  servNew = servOld-diff;

  if((servNew-servOld)<relMove)           { servNew = servOld - relMove; }
  else if((servNew-servOld)>=relMove)     { servNew = servOld + relMove; }
  if(servNew<=610)                        { servNew = 610; }
  else if(servNew>=2330)                  { servNew = 2330; }
  servo1.writeMicroseconds(servNew);
  
  servOld = servNew;
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
  for(i=49; i>0; i--)      { histPitch[i] = histPitch[i-1]; }
  histPitch[0] = Pitch;
  for(i=49; i>=0; i--)     { avgPitch = (avgPitch + histPitch[i]) / 2; }
}





