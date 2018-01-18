/**
 * @file MQTTClient.h
 *
 * @brief MQTT Client header for Linux
 *
 * Copyright (C) 2016. SK Telecom, All Rights Reserved.
 * Written 2016, by SK Telecom
 */

#ifndef _MQTT_H_
#define _MQTT_H_

#include "ThingPlug.h"

/*
 ****************************************
 * Enumerations
 ****************************************
 */


/*
 ****************************************
 * Structure Definition
 ****************************************
 */

/*
 ****************************************
 * Type Definition
 ****************************************
 */


/*
 ****************************************
 * Major Function
 ****************************************
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    /** data **/
    char* data;
    /** length **/
    size_t len;
} Content;

extern Content* gContent;

int handleMQTTSetCallbacks(tpMQTTConnectedCallback* cc, tpMQTTSubscribedCallback* sc, tpMQTTDisconnectedCallback* dc,
        tpMQTTConnectionLostCallback* clc, tpMQTTMessageDeliveredCallback* mdc, tpMQTTMessageArrivedCallback* mac);

int handleMQTTCreate(char* host, int port, int keepalive, char* userName, char* password, int enableServerCertAuth,
         char* subscribeTopic[], int subscribeTopicCnt, char* publishTopic, char* enabledCipherSuites, int cleanSession, char* clientID);

int handleMQTTSubscribe(char* topic, int qos);

int handleMQTTPublishMessage(char* payload);

int handleMQTTPublishMessageWithTopic(char* topic, char* payload);

int handleMQTTYield(unsigned long timeout_ms);

int handleMQTTIsConnected(void);

int handleMQTTDisconnect(void);

void handleMQTTDestroy(void);

#ifdef __cplusplus
}
#endif

#endif //_MQTT_H_
