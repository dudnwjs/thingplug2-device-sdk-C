/**
 * @file ThingPlug_oneM2M_SDK.c
 *
 * @brief Simple ThingPlug_oneM2M_SDK application
 *
 * Copyright (C) 2016. SK Telecom, All Rights Reserved.
 * Written 2016, by SK Telecom
 */

#include <mbed.h>

#include "NTPClient.h"
#include "MQTTEthernetExt.h"
#define NTP_RETRY_COUNT                     10
Serial pc(USBTX, USBRX);
#ifdef __cplusplus
extern "C" {
#endif
extern int ThingPlug_Simple_SDK(void);
#ifdef __cplusplus
}
#endif

/**
 * @brief main
 * @param[in] argc
 * @param[in] argv
 */
int main(int argc, char **argv) {
	pc.baud(115200);
	fprintf(stdout, " MBED START! \r\n");
	int wait_time = 2;
	while(1)
	{
        MQTTEthernetExt* g_pNetwork = new MQTTEthernetExt();
        if(!g_pNetwork->isSocketReady) {
            printf("try reconnect after %d(sec) \r\n", wait_time);
            wait_time += wait_time;
            continue;
        } else {
            NTPClient ntp;
            int loop_cnt = NTP_RETRY_COUNT;
            while(loop_cnt--) {
                if( ntp.setTime("211.233.40.78") == 0 ) {
                    time_t ctTime;
                    ctTime = time(NULL);
                    ctTime += 32400;
                    printf("Time is set to (GMT+9): %s \r\n", ctime(&ctTime));
                    break;
                }
            }
        }
        delete(g_pNetwork);
        wait(2.0);
        ThingPlug_Simple_SDK();
    }
    //return 0;
}

