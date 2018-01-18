/**
 * @file SKTtpDebug.c
 *
 * @brief Log print
 *
 * Copyright (C) 2017. SK Telecom, All Rights Reserved.
 * Written 2017, by SK Telecom
 */

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../include/SKTtpDebug.h"
#include "../include/SKTtpDebug.h"
#include "../../StreamWrapper.h"

#ifdef __cplusplus
extern "C" {
#endif
extern unsigned int getNow();
extern int getHour();
extern int getMinute();
extern int getSecond();
extern int getYear();
extern int getMonth();
extern int getDay();
#ifdef __cplusplus
}
#endif

BOOLEAN_TYPE_E  gSKTtpDebugEnable   = False;
LOG_LEVEL_E     gSKTtpDebugLogLevel = (LOG_LEVEL_E)SKT_LOG_LEVEL_NONE;

/*
 * SKTtpDebugLogLevelString
 */
void SKTtpDebugLogLevelString(LOG_LEVEL_E level, char *buffer)
{
    switch(level) {
        case SKT_LOG_LEVEL_VERBOSE: {
            memcpy(buffer, "VERBOSE", strlen("VERBOSE"));
        }
        break;

        case SKT_LOG_LEVEL_DEBUG: {
            memcpy(buffer, "DEBUG", strlen("DEBUG"));
        }
        break;

        case SKT_LOG_LEVEL_INFO: {
            memcpy(buffer, "INFO", strlen("INFO"));
        }
        break;

        case SKT_LOG_LEVEL_WARN: {
            memcpy(buffer, "WARN", strlen("WARN"));
        }
        break;

        case SKT_LOG_LEVEL_ERROR: {
            memcpy(buffer, "ERROR", strlen("ERROR"));
        }
        break;
        default: {
            memcpy(buffer, "NONE", strlen("NONE"));
        }
        break;
    }
}

/*
 * DebugInit
 */
void SKTtpDebugInit(BOOLEAN_TYPE_E enable, LOG_LEVEL_E level)
{
    gSKTtpDebugEnable   = enable;
    gSKTtpDebugLogLevel = level;
}

/*
 * TimeToString
 */
char* TimeToString(void) {
  static char s[20];

  sprintf(s, "%04d-%02d-%02d %02d:%02d:%02d",
              getYear(), getMonth(), getDay(),
              getHour(), getMinute(), getSecond()
          );

  return s;
}


/*
 * DebugPrint
 */
void SKTtpDebugPrintf(LOG_LEVEL_E level, const char *format, ...)
{
    if(gSKTtpDebugEnable == False ) {
        return;
    }

    if(gSKTtpDebugLogLevel == SKT_LOG_LEVEL_NONE) {
        return;
    }


    if(gSKTtpDebugLogLevel > level) {
        return;
    }

    char stringBuffer[30];
    memset(stringBuffer, 0, 30);
    SKTtpDebugLogLevelString(level, stringBuffer);

    char buffer[2048];
    sprintf(buffer, "[%s] [%s]: ", TimeToString(), stringBuffer);
    Stream_print_str(NULL, buffer);
    va_list argp;
    va_start(argp, format);
    vsprintf(buffer, format, argp);
    va_end(argp);
    Stream_print_str(NULL, buffer);
}

