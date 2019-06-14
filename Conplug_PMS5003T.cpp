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


#include <Arduino.h>

#include "Conplug_PMS5003T.h"


Conplug_PMS5003T::Conplug_PMS5003T(SoftwareSerial* pPmsSerial)
{
    PmsSerial = pPmsSerial;
    PmsSerial->begin(9600);
    DeviceType = PMS5003T; // default
    
    DelayValue[Conplug_PMS5003T::AFTER_SEND_PASSIVE_CMD] = 50;
    DelayValue[Conplug_PMS5003T::AFTER_SEND_REQUEST_CMD] = 50;
    DelayValue[Conplug_PMS5003T::SERIAL_READ] = 10;
}

int Conplug_PMS5003T::begin()
{
    //
    // Change mode to passive
    //
    PmsSerial->write(PMS_CMD_PASSIVE_MODE, sizeof(PMS_CMD_PASSIVE_MODE));
    delay(DelayValue[Conplug_PMS5003T::AFTER_SEND_PASSIVE_CMD]);

    Pd = readPms();

    if(Pd)
        return 1; // Success
    else
        return 0;
}

PMS5003T_DATA* Conplug_PMS5003T::readPms() {
    int bi = 0;
    unsigned char c;
    unsigned long pms_timeout = 0;
  
    //
    // Send request read
    //
    PmsSerial->write(PMS_CMD_REQUEST_READ, sizeof(PMS_CMD_REQUEST_READ));
    delay(DelayValue[Conplug_PMS5003T::AFTER_SEND_REQUEST_CMD]); // Wait for PMS5003 to calculate data.
  
    memset(&(Buff[0]), 0, MAX_PMS_DATA_SIZE);
  
    //
    // Search for the sign - {0x42, 0x4d}
    //
    pms_timeout = millis();
    while (PmsSerial->available()) {
      Buff[1] = PmsSerial->read();
      if((Buff[0] == 0x42) && (Buff[1] == 0x4d)) {
        bi = 2;
        break;
      }
      else {
        Buff[0] = Buff[1];
      }
      if((millis() - pms_timeout) > 1000) // timeout : 1 second
        break;
  
      delay(DelayValue[Conplug_PMS5003T::SERIAL_READ]); // PMS5003T is a slow sensor
    }
  
    //
    // Receive data and copy to "Buff".
    //
    while (PmsSerial->available()) {
  
      if(bi >= MAX_PMS_DATA_SIZE) break;
  
      c = PmsSerial->read();
      Buff[bi] = c;
      bi++;

      delay(DelayValue[Conplug_PMS5003T::SERIAL_READ]); // PMS5003T is a slow sensor
    }
  
    //
    // Exchange high low bytes
    //
    for(int i = 2; i < MAX_PMS_DATA_SIZE; i = i + 2) {
      uint8_t temp8 = Buff[i];
      Buff[i] = Buff[i + 1];
      Buff[i + 1] = temp8;
    }

    Pd = (PMS5003T_DATA*)(&(Buff[0]));
    
    if (Pd->DATA_LENGTH == 28)
        DeviceType = PMS5003T;
    else
        DeviceType = PMS3003;

    if((Pd->SIG_1 == 0x42) && (Pd->SIG_2 == 0x4d)) {
        return Pd;
    }
    else {
        Pd = 0;
        return Pd;
    }
}

//
// Running readPms before running pm2_5
//
int Conplug_PMS5003T::pm2_5()
{
    return Pd->PM_AE_UG_2_5;
}

//
// Running readPms before running temp
//
float Conplug_PMS5003T::temp()
{
    return ((float)(Pd->PM_TEMP)) / 10;
}

//
// Running readPms before running humi
//
float Conplug_PMS5003T::humi()
{
    return ((float)(Pd->PM_HUMI)) / 10;
}

//
// Running readPms before running readDeviceType
//
Conplug_PMS5003T::PMS_TYPE Conplug_PMS5003T::readDeviceType()
{
    return DeviceType;
}

//
// The function must be run before begin.
//
void Conplug_PMS5003T::setDelay(Conplug_PMS5003T::PMS_DELAY PmsDelay, int Value)
{
    DelayValue[PmsDelay] = Value;
}
