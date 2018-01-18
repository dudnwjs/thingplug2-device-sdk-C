/**
 * @file MQTTClient.c
 *
 * @brief MQTT Client for Linux
 *
 * Copyright (C) 2016. SK Telecom, All Rights Reserved.
 * Written 2016, by SK Telecom
 */

#include <SPI.h>
#include <Ethernet.h>
#include <string.h>
#include "../../../MQTT/IPStack.h"
#include "../../../MQTT/Countdown.h"
#include "../../../MQTT/MQTTClient.h"
#include "../../include/MQTTHandler.h"
#include "../../include/SKTtpDebug.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MQTT_MESSAGE_HANDLER_MAX_NUM	2
#define MQTT_PACKET_SIZE				1024
#define MQTT_RETRY_CNT 					10
#define MQTT_RETRY_INTERVAL				100


EthernetClient mEthernet; 
IPStack mIpstack(mEthernet);
MQTT::Client<IPStack, Countdown, MQTT_PACKET_SIZE, MQTT_MESSAGE_HANDLER_MAX_NUM> mClient = MQTT::Client<IPStack, Countdown, MQTT_PACKET_SIZE, MQTT_MESSAGE_HANDLER_MAX_NUM>(mIpstack);

static char mPublishTopic[128];
static char mSubscribeTopic[MQTT_MESSAGE_HANDLER_MAX_NUM][128];
static int mSubscribeTopicsize;

static tpMQTTConnectedCallback* mConnectedCallback;
static tpMQTTSubscribedCallback* mSubscribedCallback;
static tpMQTTDisconnectedCallback* mDisconnectedCallback;
static tpMQTTConnectionLostCallback* mConnectionLostCallback;
static tpMQTTMessageDeliveredCallback* mMessageDeliveredCallback;
static tpMQTTMessageArrivedCallback* mMessageArrivedCallback;

Content* gContent = NULL;

void OnConnect(void* context){
    if(mConnectedCallback) mConnectedCallback(MQTT::SUCCESS);
}

void OnConnectFailure(void* context){
    if(mConnectedCallback) mConnectedCallback(MQTT::FAILURE);
}

void OnSubscribe(void* context) {
    mSubscribeTopicsize--;    
	Serial.println("OnSubscribe");
    if(mSubscribedCallback && mSubscribeTopicsize == 0) mSubscribedCallback(MQTT::SUCCESS);
}

void OnSubscribeFailure(void* context) {
    if(mSubscribedCallback) mSubscribedCallback(MQTT::FAILURE);
}

void OnDisconnect(void* context){
    if(mDisconnectedCallback) mDisconnectedCallback(MQTT::SUCCESS);
}

void ConnectionLostCallback(void *context, char *cause) {
    if(mConnectionLostCallback) mConnectionLostCallback(cause);    
}

void MessageArrivedCallback(MQTT::MessageData& md)
{
	Serial.println("MessageArrivedCallback");
	MQTT::Message &message = md.message;

	if(message.dup)
		return;
	
    if(mMessageArrivedCallback) {
		mMessageArrivedCallback(md.topicName.lenstring.data, (char*)message.payload, message.payloadlen);
    }
	
}

void MessageDeliveredCallback(void *context) {
    if(mMessageDeliveredCallback) mMessageDeliveredCallback((int)0);
}

/**
 * @brief set callback function
 * @param[in] cc connect callback
 * @param[in] sc subscribe callback
 * @param[in] dc disconnect callback
 * @param[in] clc connection lost callback
 * @param[in] mdc message delivered callback
 * @param[in] mac message arrived callback
 * @return the return code of the set callbacks result
 */
int handleMQTTSetCallbacks(tpMQTTConnectedCallback* cc, tpMQTTSubscribedCallback* sc, tpMQTTDisconnectedCallback* dc,
        tpMQTTConnectionLostCallback* clc, tpMQTTMessageDeliveredCallback* mdc, tpMQTTMessageArrivedCallback* mac) {

    if(mac == NULL) {
        return -1;
    }
    
    mConnectedCallback = cc;
    mSubscribedCallback = sc;
    mDisconnectedCallback = dc;
    mConnectionLostCallback = clc;
    mMessageDeliveredCallback = mdc;
    mMessageArrivedCallback = mac;

    return 0;
}

/**
 * @brief create mqtt
 * @param[in] host the hostname or ip address of the broker to connect to.
 * @param[in] port the network port to connect to.  Usually 1883.
 * @param[in] keepalive the number of seconds after which the broker should send a PING message to the client if no other messages have been exchanged in that time. 
 * @param[in] userName authentication user name
 * @param[in] password authentication password
 * @param[in] enableServerCertAuth True/False option to enable verification of the server certificate
 * @param[in] subscribeTopic mqtt topic
 * @param[in] subscribeTopicSize mqtt topic size
 * @param[in] publishTopic publish topic
 * @param[in] enabledCipherSuites cipher format. If this setting is ommitted, its default value will be "ALL".
 * @param[in] cleanSession if cleanSession=true, then the previous session information is cleared.
 * @param[in] clientID The client identifier(no longer 23 characters).
 * @return the return code of the connection response
 */
int handleMQTTCreate(char* host, int port, int keepalive, char* userName, char* password, 
    int enableServerCertAuth, char* subscribeTopic[], int subscribeTopicSize, char* publishTopic, char* enabledCipherSuites, int cleanSession, char* clientID) {
	{
        char str[64];
        snprintf(str,64,"handleMQTTCreate");
        SKTDebugPrint(SKT_LOG_LEVEL_INFO, str);
    }
	long randNumber;		// for unique id

	char _clientID[32] = {0,};
	char clientID_prefix[] = "arduino_client_id_";
	int rc = 0; 
	uint8_t retry = 0;

	while(MQTT_RETRY_CNT > retry)
	{
		mIpstack.disconnect();
		rc = mIpstack.connect(host, port);
		if (rc == 1)  	// Ethernet Client Success : 1, Fail : 0
			break;
		{
			char str[64];
			snprintf(str,64,"%d << retry", retry);
			SKTDebugPrint(SKT_LOG_LEVEL_INFO, str);
		}
		retry++;
		delay(MQTT_RETRY_INTERVAL);
	}
	if(rc != 1)
	{
		{
			char str[64];
			snprintf(str,64,"%d", port);
			SKTDebugPrint(SKT_LOG_LEVEL_INFO, host);
			SKTDebugPrint(SKT_LOG_LEVEL_INFO, str);
		}
		OnConnectFailure(&mClient);
		return MQTT::FAILURE;;
	}

	randomSeed(analogRead(0));
	randNumber = random(50000);

	{
		char str[64];
		snprintf(str,64,"randNumber : %ld", randNumber);
		SKTDebugPrint(SKT_LOG_LEVEL_INFO, str);
	}
	if(clientID == NULL)
	{
		sprintf(_clientID, "%s%d", clientID_prefix, (int)randNumber);
		clientID = _clientID;
		{
			char str[64];
			snprintf(str,64,"ClientID : %s", clientID);
			SKTDebugPrint(SKT_LOG_LEVEL_INFO, str);
		}
	}
	

	MQTTPacket_connectData conn_opts = MQTTPacket_connectData_initializer;  
	conn_opts.MQTTVersion = 3;
	conn_opts.clientID.cstring = clientID;
	conn_opts.keepAliveInterval = keepalive;
	if(userName)
		conn_opts.username.cstring = userName;
	if(password != NULL) {
		if(password && strlen(password))
			conn_opts.password.cstring = password;
	}
	conn_opts.cleansession = cleanSession;

	retry = 0;
	while(MQTT_RETRY_CNT > retry)
	{
		rc = mClient.connect(conn_opts);
		if (rc == 0)  // success 0
			break;

		{
			char str[64];
			snprintf(str,64,"MQTT Retry : %d", retry);
			SKTDebugPrint(SKT_LOG_LEVEL_INFO, str);
		}
		retry++;
		delay(MQTT_RETRY_INTERVAL);		
	}

 	if (rc != 0)
	{
		{
			char str[64];
			snprintf(str,64,"MQTT connect failed : %d", rc);
			SKTDebugPrint(SKT_LOG_LEVEL_INFO, str);
		}
		OnConnectFailure(&mClient);
		return MQTT::FAILURE;
	}

	OnConnect(&mClient);

    mSubscribeTopicsize = subscribeTopicSize;
    for(int i = 0; i < subscribeTopicSize; i++) {
        memset(mSubscribeTopic[i], 0, sizeof(mSubscribeTopic[i]));
        strncpy(mSubscribeTopic[i], subscribeTopic[i], strlen(subscribeTopic[i]));

		{
			char str[128];
			snprintf(str,128,"MQTT Request subscibe topic : %s", mSubscribeTopic[i]);
			SKTDebugPrint(SKT_LOG_LEVEL_INFO, str);
		}
		rc = mClient.subscribe(mSubscribeTopic[i], MQTT::QOS1, MessageArrivedCallback);  
		if (rc != 0)  
		{	
			{
				char str[128];
				snprintf(str,128,"MQTT Request subscibe Failed : %s", mSubscribeTopic[i]);
				SKTDebugPrint(SKT_LOG_LEVEL_INFO, str);
			}	
			OnSubscribeFailure(&mClient);
			return MQTT::FAILURE;
		}
		else
		{
			OnSubscribe(&mClient);
		}
    }
    return MQTT::SUCCESS;
}

/**
 * @brief async subscribe
 * @param[in] topic The subscription topic, which may include wildcards.
 * @param[in] qos The requested quality of service for the subscription.
 * @return MQTTASYNC_SUCCESS if the subscription request is successful.
 */
int handleMQTTSubscribe(char* topic, int qos) {
	{
		char str[128];
		snprintf(str,128,"handleMQTTSubscribe");
		SKTDebugPrint(SKT_LOG_LEVEL_INFO, str);
	}
	int rc = MQTT::SUCCESS;

	if(!mClient.isConnected()|| topic == NULL || qos < (int)MQTT::QOS0 || qos > (int)MQTT::QOS2 )
	{
		{
			char str[128];
			snprintf(str,128,"handleMQTTSubscribe failed[1]");
			SKTDebugPrint(SKT_LOG_LEVEL_INFO, str);
		}
		return MQTT::FAILURE;
	}
	
	rc = mClient.subscribe(topic, (MQTT::QoS)qos, MessageArrivedCallback);  
	if(rc != 0)
	{
		{
			char str[128];
			snprintf(str,128,"handleMQTTSubscribe failed[2]");
			SKTDebugPrint(SKT_LOG_LEVEL_INFO, str);
		}
		OnSubscribeFailure(&mClient);
		return MQTT::FAILURE;
	}

	OnSubscribe(&mClient);
    return rc;
}

/**
 * @brief publish message
 * @param[in] payload A pointer to the payload of the MQTT message.
 * @return MQTTASYNC_SUCCESS if the message is accepted for publication.
 */
int handleMQTTPublishMessageWithTopic(char* topic, char* payload) {
	{
		char str[128];
		snprintf(str,128,"MQTTPublishMessage");
		SKTDebugPrint(SKT_LOG_LEVEL_INFO, str);
	}

    if(!mClient.isConnected() || strlen(topic) <= 0 || payload == NULL) 
	{
		{
			char str[128];
			snprintf(str,128,"handleMQTTPublishMessageWithTopic payload ->");
			SKTDebugPrint(SKT_LOG_LEVEL_INFO, str);
			SKTDebugPrint(SKT_LOG_LEVEL_INFO, payload);
		}		
        return MQTT::FAILURE;
    }

	MQTT::Message mMessage;

	mMessage.payload = (void*)payload;
	mMessage.payloadlen = strlen((char*)payload);
	mMessage.qos = MQTT::QOS1;
	mMessage.retained = false;
	mMessage.dup = false;

	{	
		char str[128];
		snprintf(str,128,"payloadlen = %d",mMessage.payloadlen);
		SKTDebugPrint(SKT_LOG_LEVEL_INFO, str);
		snprintf(str,128,"MQTT publish topic = %s", topic);
		SKTDebugPrint(SKT_LOG_LEVEL_INFO, str);		
	}
	
	int rc = mClient.publish(topic, mMessage);
	if(rc != MQTT::SUCCESS)
	{	
		{	
			char str[128];
			snprintf(str,128,"MQTT publish failed [%d]", rc);
			SKTDebugPrint(SKT_LOG_LEVEL_INFO, str);	
		}
	}
	else
	{
		{	
			char str[128];
			snprintf(str,128,"MQTT publish Success");
			SKTDebugPrint(SKT_LOG_LEVEL_INFO, str);	
		}
		MessageDeliveredCallback(&mClient);
	}

	{	
		char str[128];
		snprintf(str,128,"handleMQTTPublishMessageWithTopic END");
		SKTDebugPrint(SKT_LOG_LEVEL_INFO, str);	
	}
	return rc;
}

/**
 * @brief publish message
 * @param[in] payload A pointer to the payload of the MQTT message.
 * @return MQTTASYNC_SUCCESS if the message is accepted for publication.
 */
int handleMQTTPublishMessage(char* payload) {
	{	
		char str[128];
		snprintf(str,128,"handleMQTTPublishMessage");
		SKTDebugPrint(SKT_LOG_LEVEL_INFO, str);	
	}

    if(!mClient.isConnected() || strlen(mPublishTopic) <= 0 || payload == NULL) 
	{
		{
			char str[128];
			snprintf(str,128,"handleMQTTPublishMessage payload ->");
			SKTDebugPrint(SKT_LOG_LEVEL_INFO, str);
			SKTDebugPrint(SKT_LOG_LEVEL_INFO, payload);
		}
		
        return MQTT::FAILURE;
    }

	MQTT::Message mMessage;

	mMessage.payload = (void*)payload;
	mMessage.payloadlen = strlen((char*)payload);
	mMessage.qos = MQTT::QOS1;
	mMessage.retained = false;
	mMessage.dup = false;

	{	
		char str[128];
		snprintf(str,128,"payloadlen = %d",mMessage.payloadlen);
		SKTDebugPrint(SKT_LOG_LEVEL_INFO, str);
		snprintf(str,128,"MQTT publish topic = %s", mPublishTopic);
		SKTDebugPrint(SKT_LOG_LEVEL_INFO, str);		
	}
	
	int rc = mClient.publish(mPublishTopic, mMessage);
	if(rc != MQTT::SUCCESS)
	{
		{	
			char str[128];
			snprintf(str,128,"MQTT publish failed[1]");
			SKTDebugPrint(SKT_LOG_LEVEL_INFO, str);	
		}
	}
	else
	{
		{	
			char str[128];
			snprintf(str,128,"MQTT publish Success");
			SKTDebugPrint(SKT_LOG_LEVEL_INFO, str);	
		}
		MessageDeliveredCallback(&mClient);
	}

	{	
		char str[128];
		snprintf(str,128,"handleMQTTPublishMessage END");
		SKTDebugPrint(SKT_LOG_LEVEL_INFO, str);	
	}
	return rc;
}


/**
 * @brief disconnect mqtt
 */
int handleMQTTDisconnect() {
	{	
		char str[128];
		snprintf(str,128,"handleMQTTDisconnect");
		SKTDebugPrint(SKT_LOG_LEVEL_INFO, str);	
	}
    int rc = MQTT::SUCCESS;
	if(mClient.isConnected())
	{
		rc = mClient.disconnect();
		if( rc == MQTT::SUCCESS)
			OnDisconnect(&mClient);

		mIpstack.disconnect();
	}
    return (int)rc;
}

/**
 * @brief destroy mqtt
 */
void handleMQTTDestroy() {
	{	
		char str[128];
		snprintf(str,128,"handleMQTTDestroy");
		SKTDebugPrint(SKT_LOG_LEVEL_INFO, str);	
	}
    if(gContent) {
        if(gContent->data) {
            free(gContent->data);
            gContent->data = NULL;
        }
        free(gContent);
        gContent = NULL;
    } 
    // disconnect when connected.
    if(mClient.isConnected()) {
        handleMQTTDisconnect();
	}
}

/**
 * @brief is connected
 * @return true : connected <-> false
 */
int handleMQTTIsConnected() {
    return  mClient.isConnected();
}


int handleMQTTYield(unsigned long timeout_ms)
{
	return mClient.yield(timeout_ms);
}


#ifdef __cplusplus
}
#endif
