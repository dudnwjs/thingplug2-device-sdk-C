/**
   @file Simple.ino

   @brief MangementAgent

   Copyright (C) 2017. SK Telecom, All Rights Reserved.
   Written 2017, by SK Telecom
*/

#include <SPI.h>
#include <Ethernet.h>
#include <TimeLib.h>

#ifdef __cplusplus
extern "C" {
#endif
extern int ThingPlug_Simple_SDK(void);
extern void setNtpTime();
#ifdef __cplusplus
}
#endif

char curr_ip[30];  
char curr_gw[30];  

void setup()
{
  byte mac[] = {0x90, 0xA2, 0xDA, 0x10, 0x30, 0xDF}; // ex :
  Serial.begin(9600);
  Serial.println("MA Start");
  Ethernet.begin(mac);
  setNtpTime();
}

void loop()
{
  IPAddress myAddr = Ethernet.localIP();
  unsigned char oct1 = myAddr[0];
  unsigned char oct2 = myAddr[1];
  unsigned char oct3 = myAddr[2];
  unsigned char oct4 = myAddr[3];
  sprintf(curr_ip, "%d.%d.%d.%d", oct1, oct2, oct3, oct4);
  myAddr = Ethernet.gatewayIP();
  oct1 = myAddr[0];
  oct2 = myAddr[1];
  oct3 = myAddr[2];
  oct4 = myAddr[3];
  sprintf(curr_gw, "%d.%d.%d.%d", oct1, oct2, oct3, oct4);
  ThingPlug_Simple_SDK();
  Ethernet.maintain();
}

