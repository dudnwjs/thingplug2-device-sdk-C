/**
 * @file SKTtpDebug.h
 *
 * @brief Log print header file
 *
 * Copyright (C) 2016. SK Telecom, All Rights Reserved.
 * Written 2016, by SK Telecom
 */

#ifndef _LOG_PRINT_H_
#define _LOG_PRINT_H_

#include "StreamWrapper.h"

typedef enum tagLogLevel {
	SKT_LOG_LEVEL_NONE =0,
	SKT_LOG_LEVEL_VERBOSE,
	SKT_LOG_LEVEL_DEBUG,
	SKT_LOG_LEVEL_INFO,
	SKT_LOG_LEVEL_WARN,
	SKT_LOG_LEVEL_ERROR,
	SKT_LOG_LEVEL_FATAL,
	SKT_LOG_LEVEL_MAX
} LOG_LEVEL_E;

typedef enum tagBooleanType {
	False = 0,
	True	
} BOOLEAN_TYPE_E;

#define DEBUG_ENABLE
#ifdef DEBUG_ENABLE
	#include <stdio.h>
extern void SKTtpDebugInit(BOOLEAN_TYPE_E enable, LOG_LEVEL_E level);
extern void SKTtpDebugPrintf( LOG_LEVEL_E level, char *str);

#define SKTDebugInit(enable, level) SKTtpDebugInit(enable, level)
#define SKTDebugPrint             SKTtpDebugPrintf
#else
	#define SKTDebugInit(enable, level, stream)        ((void)0)
	#define SKTDebugPrint(...)                           ((void)0)
#endif

#endif /* _LOG_PRINT_H_ */
