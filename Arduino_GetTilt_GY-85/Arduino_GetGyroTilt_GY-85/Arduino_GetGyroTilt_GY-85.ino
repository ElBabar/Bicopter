#include <ADXL345.h>
#include <Wire.h>
#include <ITG3200.h>

ADXL345 acc;
ITG3200 gyro = ITG3200(); // définition du type gyro
int time, dtime = 0;
float  gx, gy, gz;
float angleGyroX, angleGyroY, angleGyroZ;

float histAngularX[50]={0};
float avgAngularX = 0;
float histAngularY[50]={0};
float avgAngularY = 0;


void setup() {
  
  
  Serial.begin(115200);
  acc.powerOn();
  
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
  
  GetGyroAngle();
 
  
  //gyro.readGyro(&gx,&gy,&gz); 
  //averageAngularX();
  //averageAngularY();

  
  Serial.print(angleGyroX);
  Serial.print(" , ");
  Serial.println(angleGyroY);
  
  //Serial.println(dtime);
}



void GetGyroAngle(void) {
  // calcul de l'angle depuis la vitesse angulaire
   gyro.readGyro(&gx,&gy,&gz); 
   dtime = millis() - time;
  angleGyroX += gx * dtime /1000;
  angleGyroY += gy * dtime /1000;
  angleGyroZ += gz * dtime /1000;
   time = millis();
}

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







