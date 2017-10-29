#include <Wire.h>
#include <ADXL345.h>  // ADXL345 Accelerometer Library

ADXL345 acc; //variable adxl is an instance of the ADXL345 library
int ax, ay, az;
float rawX, rawY, rawZ, dtime;
float angleAccX, angleAccY;
int time=0;

void setup() {
  Serial.begin(115200);
  acc.powerOn();
  acc.setRangeSetting(2);
}

void loop() {
  dtime = millis()-time;
  time=millis();
  GetAccTilt();

  //Serial.print(angleAccX/100);
  //Serial.print(" , ");
  Serial.print(ay);
  Serial.print("   ");
  Serial.print(rawY);
  Serial.print("   ");
  Serial.println(angleAccY);

}

void GetAccTilt(void)
{
  acc.readAccel(&ax, &ay, &az); //read the accelerometer values and store them in variables  x,y,z
  rawX = ax + 37;
  rawY = ay + 256;
  rawZ = az - 326;
  
if(rawZ >= 0)               { angleAccX = 100*(-180 * (atan(rawX / rawZ)) / PI); }
if((rawZ < 0) && (rawX>=0)) { angleAccX = 100*(-180 + (-180 * (atan(rawX / rawZ)) / PI)); }
if((rawZ < 0) && (rawX<0))  { angleAccX = 100*(180 + (-180 * (atan(rawX / rawZ)) / PI)); }

if(rawZ >= 0)               { angleAccY = 100*(-180 * (atan(rawY / rawZ)) / PI); }
if((rawZ < 0) && (rawY>=0)) { angleAccY = 100*(-180 + (-180 * (atan(rawY / rawZ)) / PI)); }
if((rawZ < 0) && (rawY<0))  { angleAccY = 100*(180 + (-180 * (atan(rawY / rawZ)) / PI)); }
  
  //angleAccY = 180 * (atan(rawY / rawZ)) / PI;
}

