
/**
 * @file SMA.c
 *
 * @brief SensorManagementAgent Process
 *
 * Copyright (C) 2016. SKT, All Rights Reserved.
 * Written 2016,by SKT
 */

#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "SMA.h"

#ifdef USE_LCD
#include "rgb_lcd.h"
#endif


#ifdef USE_PROX
unsigned long GetProximityValue() {
    unsigned long echo = 0;
    unsigned long value = 0;
    pinMode(PROX_PIN, OUTPUT); // Switch signalpin to output
    digitalWrite(PROX_PIN, LOW); // Send low pulse 
    delayMicroseconds(2); // Wait for 2 microseconds
    digitalWrite(PROX_PIN, HIGH); // Send high pulse
    delayMicroseconds(5); // Wait for 5 microseconds
    digitalWrite(PROX_PIN, LOW); // Holdoff
    pinMode(PROX_PIN, INPUT); // Switch signalpin to input
    digitalWrite(PROX_PIN, HIGH); // Turn on pullup resistor
    // please note that pulseIn has a 1sec timeout, which may
    // not be desirable. Depending on your sensor specs, you
    // can likely bound the time like this -- marcmerlin
    // echo = pulseIn(distPin, HIGH, 38000)
    echo = pulseIn(PROX_PIN, HIGH); //Listen for echo
    value = (echo/2) / 29.1;
    return value;
}
#endif

#define SMA_STRCMP(x,y) (strncmp(x,y,strlen(y)) == 0)&&(strlen(x) == strlen(y))

void SMAGetData(char *sensorType, char** output, int *len)
{
	randomSeed(analogRead(0));
    int random_num = random(9999);;

    if( SMA_STRCMP(sensorType,"batterystate") ) {
        *output = strdup("1");
        *len = strlen(*output);
    } else if( SMA_STRCMP(sensorType, "temp1") ) {
#ifndef __DUMMY_DATA_
        char temp[10] = "";
        int base = analogRead(TEMP_PIN);
        float R = 1023.0/((float)base)-1.0; 
        int B = 4275;
        R = 100000.0 * R;
        float r = 1.0/(log(R/100000.0)/B+1/298.15)-273.15;            
        snprintf(temp, sizeof(temp), "%d", (int)(r/2)); // half value when 5V
        *output = strdup(temp);
        *len = strlen(*output);
#else
        *output = strdup("XX.XX");
        sprintf(*output, "%.2f", (random_num%5000+2000)/100.0);
        *len = strlen(*output);
#endif
    } else if( SMA_STRCMP(sensorType, "humi1") ) {
        *output = strdup("XX");
        sprintf(*output, "%2d",(random_num%10+40));
        *len = strlen(*output);
    } else if( SMA_STRCMP(sensorType, "light1") ) {
#ifndef __DUMMY_DATA_
        char light[10] = "";
        int r = analogRead(LIGHT_PIN)/2;
        snprintf(light, sizeof(light), "%d", r/2); // half value when 5V 
        *output = strdup(light);
		*len = strlen(*output);
#else
        *output = strdup("XXX");
        sprintf(*output, "%3d", (random_num%50+200));
        *len = strlen(*output);
#endif
    } else {
        *output = NULL;
    }
}

void SMASetLED(int pin, int flag)
{
	if( flag == 0){
		digitalWrite(pin, LOW);
	} else {
		digitalWrite(pin, HIGH);
	}
}

#ifdef USE_LCD
rgb_lcd lcd;

void SMAInitLCD(int x, int y)
{
	lcd.begin(x, y); //16, 2
}
void SMASetLCDRGB(int R, int G, int B)
{
	lcd.setRGB(R, G, B);
}
void SMASetLCD(int x, int y, char *str, int clear_flag)
{
	if( clear_flag == 1 ) {
		lcd.clear();
	}
	lcd.setCursor(x, y);
	lcd.print(str);
}
#endif

static int ledStatus = 0;

int RGB_LEDControl(int color)
{
    // r=1,g=2,b=3,c=5,m=4,y=6,w=7,off=0
    if( color == 1 ) {
        ledStatus = 1;
#ifndef __DUMMY_DATA_
		SMASetLED(LED_PIN, 1);
#endif
    } else if( color == 2 ) {
        ledStatus = 2;
    } else if( color == 3 ) {
        ledStatus = 3;
    } else if( color == 4 ) {
        ledStatus = 4;
    } else if( color == 5 ) {
        ledStatus = 5;
    } else if( color == 6 ) {
        ledStatus = 6;
    } else if( color == 7 ) {
        ledStatus = 7;
    } else if( color == 0 ) {
        ledStatus = 0;
#ifndef __DUMMY_DATA_
		SMASetLED(LED_PIN, 0);
#endif
    } else {
        return -1;
    }
    return 0;
}

int RGB_LEDStatus(void) 
{
    return ledStatus;
}

