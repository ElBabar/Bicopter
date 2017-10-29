#include <Wire.h>
#include <ADXL345.h>  // ADXL345 Accelerometer Library
#include <ITG3200.h>

ADXL345 acc; //variable adxl is an instance of the ADXL345 library
ITG3200 gyro = ITG3200(); // définition du type gyro
int ax, ay, az;
float rawX, rawY, rawZ, dtime;
float angleAccX, angleAccY;
float  gx, gy, gz;
float angleGyroX, angleGyroY, angleGyroZ;
float tima=0, Pitch, Roll;

float compX, compY, compZ;

void setup() {
  Serial.begin(115200);
  acc.powerOn();
  acc.setRangeSetting(8);
  Serial.println("Accelerometer started !");
  delay(100);
  Serial.println("Starting...");
  Serial.println("Gyro Init...");
  gyro.init(ITG3200_ADDR_AD0_LOW);
  Serial.println("Gyro Calibrate...");
  gyro.zeroCalibrate(500, 1);
  Serial.println("Setup success !");
  delay(1000);
}

void loop() {

  GetAllTilt();
  
  Serial.print("compX : ");
  Serial.print(compX);
  Serial.print("  compY : ");
  Serial.println(compY);


}

void GetAllTilt(void) {
  GetAccTilt();
  GetGyroAngle();
  
  compX = 0.98*(compX)+0.02*(-angleAccY/100);
  compY = 0.98*(compY)+0.02*(-angleAccX/100);

  Pitch = -compX;
  Roll = compY;
}

void GetGyroAngle(void) {
  // calcul de l'angle depuis la vitesse angulaire
   gyro.readGyro(&gx,&gy,&gz); 
   dtime = millis() - tima;
  compX += gx * dtime /1000;
  compY += gy * dtime /1000;
  angleGyroZ += gz * dtime /1000;
   tima = millis();
}


void GetAccTilt(void)
{
  acc.readAccel(&ax, &ay, &az); //read the accelerometer values and store them in variables  x,y,z
  rawX = ax + 37;
  rawY = ay + 62;
  rawZ = az - 326;
  
if(rawZ >= 0)               { angleAccX = 100*(-180 * (atan(rawX / rawZ)) / PI); }
if((rawZ < 0) && (rawX>=0)) { angleAccX = 100*(-180 + (-180 * (atan(rawX / rawZ)) / PI)); }
if((rawZ < 0) && (rawX<0))  { angleAccX = 100*(180 + (-180 * (atan(rawX / rawZ)) / PI)); }

if(rawZ >= 0)               { angleAccY = 100*(-180 * (atan(rawY / rawZ)) / PI); }
if((rawZ < 0) && (rawY>=0)) { angleAccY = 100*(-180 + (-180 * (atan(rawY / rawZ)) / PI)); }
if((rawZ < 0) && (rawY<0))  { angleAccY = 100*(180 + (-180 * (atan(rawY / rawZ)) / PI)); }
  
  //angleAccY = 180 * (atan(rawY / rawZ)) / PI;
}

