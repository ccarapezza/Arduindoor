#include <Wire.h>
#include <Arduino.h>

void i2cSetup() {
  Wire.begin();
}

void sendI2CMessage(char idFunction, int params[], int paramsCount){
  Wire.beginTransmission(1);
  Wire.write(idFunction);
  
  for (int i=0; i < paramsCount; i++) {
    Serial.print("Write Param ");
    Serial.println(i);
    Wire.write(params[i]);
  }

  // Paramos la transmisión
  Wire.endTransmission();
}