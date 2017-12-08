/**
   @file ArduinoMiddleware.ino

   @brief MangementAgent

   Copyright (C) 2016. SK Telecom, All Rights Reserved.
   Written 2016, by SK Telecom
*/

#include <SPI.h>
#include <Ethernet.h>

#include "src/MA/MA.h"
#include "src/SMA/GetTime.h"

void setup()
{
  byte mac[] = {0x90, 0xA2, 0xDA, 0x10, 0x30, 0xDF}; // ex :
  Serial.begin(9600);
  Serial.println("MA Start");
  Ethernet.begin(mac);
  setNtpTime();
  digitalClockDisplay();
}

void loop()
{
  char ip[30];  
  char gw[30];  
  IPAddress myAddr = Ethernet.localIP();
  unsigned char oct1 = myAddr[0];
  unsigned char oct2 = myAddr[1];
  unsigned char oct3 = myAddr[2];
  unsigned char oct4 = myAddr[3];
  sprintf(ip, "%d.%d.%d.%d", oct1, oct2, oct3, oct4);
  myAddr = Ethernet.gatewayIP();
  oct1 = myAddr[0];
  oct2 = myAddr[1];
  oct3 = myAddr[2];
  oct4 = myAddr[3];
  sprintf(gw, "%d.%d.%d.%d", oct1, oct2, oct3, oct4);
  MARun(ip,gw);
  Ethernet.maintain();
}

