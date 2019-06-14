//
// Copyright (c) 2019 Conplug (https://conplug.com.tw)
// Author: Hartman Hsieh
//
// Description :
//   Dust sensor - PMS5003T or PMS3003
//   Environment:
//     NodeMCU 1.0
//     Arduino 1.8.9
//
// Connections :
//   PMS5003T => Serial Port
//
// Required Library :
//   None
//


#include "Conplug_PMS5003T.h"

#include "SoftwareSerial.h"
SoftwareSerial SerialSensor(D7, D8); // RX, TX

Conplug_PMS5003T Pms(&SerialSensor);

void setup() {
  Serial.begin(9600);

  //
  // Sensors must be initialized later.
  //
  delay(3000);

  //Pms.setDelay(Conplug_PMS5003T::SERIAL_READ, 10);

  if(Pms.begin()) {
    Serial.println("PMSX003 initialize successfully.");
  }
  else {
    Serial.println("PMSX003 initialize unsuccessfully.");
  }
}

void loop() {
  PMS5003T_DATA* pd = 0;

  //
  // Running readPms before running pm2_5, temp, humi and readDeviceType.
  //
  if(pd = Pms.readPms()) {
    Serial.print("readPm2_5=");
    Serial.println(Pms.pm2_5());
    Serial.print("readTemp=");
    Serial.println(Pms.temp());
    Serial.print("readHumi=");
    Serial.println(Pms.humi());

    if(Pms.readDeviceType() == Conplug_PMS5003T::PMS5003T)
      Serial.println("PMS5003T is detected.");
    else
      Serial.println("PMS3003 is detected.");
  }
  else {
    Serial.println("PMS data format is wrong.");
  }

  //delay(2000);
}
