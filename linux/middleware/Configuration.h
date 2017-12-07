/**
 * @file Configuration.h
 *
 * @brief Configuration header for The Samples
 *
 * Copyright (C) 2017. SK Telecom, All Rights Reserved.
 * Written 2017, by SK Telecom
 */

#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

#define MQTT_HOST                           "218.53.242.111" // dodam : 218.53.242.111, IDC : 223.39.123.140
#define MQTT_SECURE_HOST                    "ssl://218.53.242.111"
#define MQTT_PORT                           1883
#define MQTT_SECURE_PORT                    8883
#define MQTT_KEEP_ALIVE                     120
#define MQTT_ENABLE_SERVER_CERT_AUTH        0

#define JSON_TELEMETRY
//#define CSV_TELEMETRY
#define LOGIN_NAME                          "(TBD)" // device token(Check with ThingPlug Portal)

#define SIMPLE_SERVICE_NAME                 "(TBD)" // service name(Check with ThingPlug Portal)
#define SIMPLE_DEVICE_NAME                  "(TBD)" // device name(Check with ThingPlug Portal)

#endif // _CONFIGURATION_H_
