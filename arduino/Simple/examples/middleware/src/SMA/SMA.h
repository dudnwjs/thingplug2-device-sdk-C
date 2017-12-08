#ifndef _SMA_H_
#define _SMA_H_

#ifdef __cplusplus
extern "C" {
#endif

//#define USE_PROX
//#define USE_LCD
//#define __DUMMY_DATA_

#ifdef USE_LCD
void SMAInitLCD(int x, int y);
void SMASetLCDRGB(int R, int G, int B);
void SMASetLCD(int x, int y,char *str, int clear_flag);
#endif

#ifndef __DUMMY_DATA_
#define TEMP_PIN  0
#define LIGHT_PIN 3
#define PROX_PIN 8
#define LED_PIN 4
#endif

void SMAGetData(char* sensor, char** output, int *len);
void SMASetLED(int pin, int flag);
int RGB_LEDStatus(void);
int RGB_LEDControl(int color);

#ifdef __cplusplus
}
#endif

#endif//_SMA_H_
