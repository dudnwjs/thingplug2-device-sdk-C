
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <MQTT/MQTTClient.h>
#include <SimpleAPI/include/MQTTHandler.h>
#include <SimpleAPI/include/SKTtpDebug.h>

#define IMPORT_TLS_LIB          // whether import TLS library

#define _MQTT_HANDLE_DEBUG_

#define MAC_ADDRESS                         { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }      // MAC address for NIC

#define MQTT_SUBSCRIBE_TOPIC_MAX_CNT        5               // subscribe topic max count
#define MQTT_TOPIC_MAX_LENGTH               128             // topic message max length

#define MQTT_UNIQUE_ID_PREFIX               "threadX_id_"

#define MQTT_MAX_PACKET_SIZE                2048
#define MQTT_RETRY_CNT                      3
#define MQTT_RETRY_INTERVAL                 100

char                            g_szPublishTopic[MQTT_TOPIC_MAX_LENGTH];
char                            g_szSubscribeTopic[MQTT_SUBSCRIBE_TOPIC_MAX_CNT][MQTT_TOPIC_MAX_LENGTH];

Client*                         g_pMQTTClient = NULL;
int                             g_enableServerCertAuth;

tpMQTTConnectedCallback*        g_pConnectedCallback;
tpMQTTSubscribedCallback*       g_pSubscribedCallback;
tpMQTTDisconnectedCallback*     g_pDisconnectedCallback;
tpMQTTConnectionLostCallback*   g_pConnectionLostCallback;
tpMQTTMessageDeliveredCallback* g_pMessageDeliveredCallback;
tpMQTTMessageArrivedCallback*   g_pMessageArrivedCallback;


Content* gContent = NULL;

void CallConnectedCallback(int result)
{
    if(g_pConnectedCallback)    { g_pConnectedCallback(result); }
}

void CallSubscribedCallback(int result)
{
    if(g_pSubscribedCallback)   { g_pSubscribedCallback(result); }
}

void CallDisconnectedCallback(int result)
{
    if(g_pDisconnectedCallback) { g_pDisconnectedCallback(result); }
}

void CallConnectionLostCallback(char *cause)
{
    if(g_pConnectionLostCallback)   { g_pConnectionLostCallback(cause); }
}

void CallMessageArrivedCallback(MessageData* md)
{
    if(md->message->dup)
    {
        return;
    }

    if(g_pMessageArrivedCallback)
    {
        MQTTMessage* message = md->message;
        g_pMessageArrivedCallback(md->topicName->lenstring.data, (char*)message->payload, message->payloadlen);
    }
}

void CallMessageDeliveredCallback()
{
    if(g_pMessageDeliveredCallback) { g_pMessageDeliveredCallback(0); }
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
        tpMQTTConnectionLostCallback* clc, tpMQTTMessageDeliveredCallback* mdc, tpMQTTMessageArrivedCallback* mac)
{
    SKTDebugPrint(LOG_LEVEL_INFO, "handleMQTTSetCallbacks()");

    if(mac == NULL)
    {
        return FAILURE;
    }

    g_pConnectedCallback = cc;
    g_pSubscribedCallback = sc;
    g_pDisconnectedCallback = dc;
    g_pConnectionLostCallback = clc;
    g_pMessageDeliveredCallback = mdc;
    g_pMessageArrivedCallback = mac;

    return SUCCESS;
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
 * @param[in] subscribeTopicCnt mqtt topic count
 * @param[in] publishTopic publish topic
 * @param[in] enabledCipherSuites cipher format. If this setting is ommitted, its default value will be "ALL".
 * @param[in] cleanSession if cleanSession=true, then the previous session information is cleared.
 * @param[in] clientID The client identifier(no longer 23 characters).
 * @return the return code of the connection response
 */
int handleMQTTCreate(char* host, int port, int keepalive, char* userName, char* password, int enableServerCertAuth,
         char* subscribeTopic[], int subscribeTopicCnt, char* publishTopic, char* enabledCipherSuites, int cleanSession, char* clientID)
{
    SKTDebugPrint(LOG_LEVEL_INFO, "handleMQTTCreate()");
    {
        char str[128];
        snprintf(str,128," host = %s", host);
        SKTDebugPrint(LOG_LEVEL_INFO,str);
        snprintf(str,128," port = %d", port);
        SKTDebugPrint(LOG_LEVEL_INFO,str);
        snprintf(str,128," keepalive = %d", keepalive);
        SKTDebugPrint(LOG_LEVEL_INFO,str);
        snprintf(str,128," userName = %s", userName);
        SKTDebugPrint(LOG_LEVEL_INFO,str);
        snprintf(str,128," password = %p", password);
        SKTDebugPrint(LOG_LEVEL_INFO,str);
        snprintf(str,128," enableServerCertAuth = %d", enableServerCertAuth);
        SKTDebugPrint(LOG_LEVEL_INFO,str);
        snprintf(str,128," subscribeTopic = %s", subscribeTopic[0]);
        SKTDebugPrint(LOG_LEVEL_INFO,str);
        snprintf(str,128," subscribeTopicCnt = %d", subscribeTopicCnt);
        SKTDebugPrint(LOG_LEVEL_INFO,str);
        snprintf(str,128," publishTopic = %p", publishTopic);
        SKTDebugPrint(LOG_LEVEL_INFO,str);
        snprintf(str,128," enabledCipherSuites = %p", enabledCipherSuites);
        SKTDebugPrint(LOG_LEVEL_INFO,str);
        snprintf(str,128," cleanSession = %d", cleanSession);
        SKTDebugPrint(LOG_LEVEL_INFO,str);
        snprintf(str,128," clientID = %s", clientID);
        SKTDebugPrint(LOG_LEVEL_INFO,str);
    }



    if(!enableServerCertAuth)   { SKTDebugPrint(LOG_LEVEL_INFO, "Use normal socket"); }
    else                        { SKTDebugPrint(LOG_LEVEL_INFO, "Use secure(TLS) socket"); }

    if(subscribeTopicCnt > MQTT_SUBSCRIBE_TOPIC_MAX_CNT)
    {
        SKTDebugPrint(LOG_LEVEL_ERROR, "Too many MQTT topics.");
        return FAILURE;
    }

    g_enableServerCertAuth = enableServerCertAuth;

    if(g_pMQTTClient == NULL)
    {
        Network* pNetwork = (Network*)malloc(sizeof(Network));

        if(!g_enableServerCertAuth) { NewNetwork(pNetwork); }
#ifdef IMPORT_TLS_LIB
        else                        { NewNetworkTLS(pNetwork); }
#else
        else
        {
            free(pNetwork);
            SKTDebugPrint(LOG_LEVEL_ERROR, "Not support TLS socket.");
            return FAILURE;
        }
#endif

        unsigned char* szSendbuf = (unsigned char*)malloc(MQTT_MAX_PACKET_SIZE * sizeof(unsigned char));
        unsigned char* szReadbuf = (unsigned char*)malloc(MQTT_MAX_PACKET_SIZE * sizeof(unsigned char));

        g_pMQTTClient = (Client*)malloc(sizeof(Client));
        MQTTClient(g_pMQTTClient, pNetwork, 30000, szSendbuf, MQTT_MAX_PACKET_SIZE, szReadbuf, MQTT_MAX_PACKET_SIZE);
    }

    int rc = SUCCESS;
    uint8_t retry = 0;

    while(MQTT_RETRY_CNT > retry)
    {
        if(!g_enableServerCertAuth) { rc = ConnectNetwork(g_pMQTTClient->ipstack, host, port); }
#ifdef IMPORT_TLS_LIB
        else                        { rc = ConnectNetworkTLS(g_pMQTTClient->ipstack, host, port); }
#endif
        if(rc == 0)
        {
            break;
        }

        SKTDebugPrint(LOG_LEVEL_ERROR, "Ethernet connect retry.");
        retry++;
        tx_thread_sleep (MQTT_RETRY_INTERVAL);
    }
    if(rc != 0)
    {
        SKTDebugPrint(LOG_LEVEL_ERROR, "Ethernet connect fail! ");
        return rc;
    }

    char uniqueid[32] = {0,};
    if(clientID == NULL || strlen(clientID) <= 0)
    {
        char clientID_prefix[] = MQTT_UNIQUE_ID_PREFIX;
        int randNumber = rand();        // for unique id
        snprintf(uniqueid,32, "%s%d", clientID_prefix, randNumber);
        clientID = uniqueid;
    }
    SKTDebugPrint(LOG_LEVEL_INFO, "clientID =>");
    SKTDebugPrint(LOG_LEVEL_INFO, clientID);

    MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;
    connectData.MQTTVersion = 3;
    connectData.clientID.cstring = clientID;
    connectData.keepAliveInterval = keepalive;
    if(userName)    { connectData.username.cstring = userName; }
    if(password != NULL) {
        if(password && strlen(password))    { connectData.password.cstring = password; }
    }
    connectData.cleansession = cleanSession;

    retry = 0;
    while(MQTT_RETRY_CNT > retry)
    {
        if((rc = MQTTConnect(g_pMQTTClient, &connectData)) == SUCCESS)
        {
            break;
        }

        SKTDebugPrint(LOG_LEVEL_ERROR, "MQTT connect retry.");
        retry++;
        tx_thread_sleep (MQTT_RETRY_INTERVAL);
    }
    if(rc != SUCCESS)
    {
        SKTDebugPrint(LOG_LEVEL_ERROR, "MQTT connect Failed");
        CallConnectedCallback(FAILURE);
        return rc;
    }

    SKTDebugPrint(LOG_LEVEL_INFO, "MQTT Connected");
    CallConnectedCallback(SUCCESS);

    memset(g_szPublishTopic, 0, MQTT_TOPIC_MAX_LENGTH);
    memcpy(g_szPublishTopic, publishTopic, strlen(publishTopic));

    for(int nLoop1 = 0 ; nLoop1 < subscribeTopicCnt ; nLoop1++)
    {
        memset(g_szSubscribeTopic[nLoop1], 0, MQTT_TOPIC_MAX_LENGTH);
        memcpy(g_szSubscribeTopic[nLoop1], subscribeTopic[nLoop1], strlen(subscribeTopic[nLoop1]));

        SKTDebugPrint(LOG_LEVEL_INFO, "MQTT Request Subscribe topic => ");
        SKTDebugPrint(LOG_LEVEL_INFO, g_szSubscribeTopic[nLoop1]);
        if((rc = MQTTSubscribe(g_pMQTTClient, g_szSubscribeTopic[nLoop1], QOS1, CallMessageArrivedCallback)) != SUCCESS)
        {
            SKTDebugPrint(LOG_LEVEL_ERROR, "MQTT subscribe Failed.");
            break;
        }
    }

    CallSubscribedCallback(rc);

    return rc;
}

/**
 * @brief async subscribe
 * @param[in] topic The subscription topic, which may include wildcards.
 * @param[in] qos The requested quality of service for the subscription.
 * @return MQTTASYNC_SUCCESS if the subscription request is successful.
 */
int handleMQTTSubscribe(char* topic, int qos)
{
    SKTDebugPrint(LOG_LEVEL_INFO, "handleMQTTSubscribe()");

    if(g_pMQTTClient == NULL || !IsNetworkConnected(g_pMQTTClient->ipstack) || topic == NULL || qos < QOS0 || qos > QOS2)
    {
        SKTDebugPrint(LOG_LEVEL_ERROR, "MQTT subscribe Failed. Parameter invalid.");
        return FAILURE;
    }

    int rc = SUCCESS;
    if((rc = MQTTSubscribe(g_pMQTTClient, topic, qos, CallMessageArrivedCallback)) != SUCCESS)
    {
        SKTDebugPrint(LOG_LEVEL_ERROR, "MQTT subscribe Failed =>");
        SKTDebugPrint(LOG_LEVEL_ERROR, topic);
    }

    CallSubscribedCallback(rc);
    return rc;
}

/**
 * @brief publish message
 * @param[in] payload A pointer to the payload of the MQTT message.
 * @return MQTTASYNC_SUCCESS if the message is accepted for publication.
 */
int handleMQTTPublishMessage(char* payload)
{
    SKTDebugPrint(LOG_LEVEL_INFO, "handleMQTTPublishMessage()");

    if(g_pMQTTClient == NULL || !IsNetworkConnected(g_pMQTTClient->ipstack) || strlen(g_szPublishTopic) <= 0 || payload == NULL)
    {
        SKTDebugPrint(LOG_LEVEL_ERROR, "MQTTPublishMessage Failed. Parameter invalid.");
//        SKTDebugPrint(LOG_LEVEL_ERROR, "mClient.isConnected : (%d)", IsNetworkConnected(g_pMQTTClient->ipstack));
        SKTDebugPrint(LOG_LEVEL_ERROR, "mPublishTopic => ");
        SKTDebugPrint(LOG_LEVEL_ERROR, g_szPublishTopic);
//        SKTDebugPrint(LOG_LEVEL_ERROR, "payload : (%s)", payload);

        return FAILURE;
    }

    MQTTMessage mMessage;
    mMessage.payload = (void*)payload;
    mMessage.payloadlen = strlen(payload);
    mMessage.qos = QOS1;
    mMessage.retained = 0;
    mMessage.dup = 0;
#ifdef _MQTT_HANDLE_DEBUG_
    char str[128];
    snprintf(str,128,"MQTT publish call.");
    SKTDebugPrint(LOG_LEVEL_DEBUG, str);
    snprintf(str,128,"topic : %s", g_szPublishTopic);
    SKTDebugPrint(LOG_LEVEL_DEBUG, str);
    snprintf(str,128,"payload : %s", mMessage.payload);
    SKTDebugPrint(LOG_LEVEL_DEBUG, str);
    snprintf(str,128,"payloadlen : %d", mMessage.payloadlen);
    SKTDebugPrint(LOG_LEVEL_DEBUG, str);
    snprintf(str,128,"qos : %d", mMessage.qos);
    SKTDebugPrint(LOG_LEVEL_DEBUG, str);
    snprintf(str,128,"retained : %d", mMessage.retained);
    SKTDebugPrint(LOG_LEVEL_DEBUG, str);
    snprintf(str,128,"dup : %d", mMessage.dup);
    SKTDebugPrint(LOG_LEVEL_DEBUG, str);
#endif
    int rc = SUCCESS;
    if((rc = MQTTPublish(g_pMQTTClient, g_szPublishTopic, &mMessage)) != SUCCESS)
    {
        SKTDebugPrint(LOG_LEVEL_ERROR, "MQTT publish Failed.");
    }
    else
    {
        CallMessageDeliveredCallback();
    }

    return rc;
}

/**
 * @brief publish message with topic
 * @param[in] topic publish topic
 * @param[in] payload A pointer to the payload of the MQTT message.
 * @return MQTTASYNC_SUCCESS if the message is accepted for publication.
 */
 int handleMQTTPublishMessageWithTopic(char* topic, char* payload) {
     SKTDebugPrint(LOG_LEVEL_INFO, "handleMQTTPublishMessageWithTopic()");

     if(g_pMQTTClient == NULL || !IsNetworkConnected(g_pMQTTClient->ipstack) || strlen(topic) <= 0 || payload == NULL)
     {
         SKTDebugPrint(LOG_LEVEL_ERROR, "MQTTPublishMessage Failed. Parameter invalid.");
#ifdef _MQTT_HANDLE_DEBUG_
         {
             char str[128];
             snprintf(str,128,"mClient.isConnected : (%d)", IsNetworkConnected(g_pMQTTClient->ipstack));
             SKTDebugPrint(LOG_LEVEL_ERROR, str);
             snprintf(str,128,"mPublishTopic => ");
             SKTDebugPrint(LOG_LEVEL_ERROR, str);
             SKTDebugPrint(LOG_LEVEL_ERROR, topic);
             SKTDebugPrint(LOG_LEVEL_ERROR, payload);
         }
#endif
         return FAILURE;
     }

     MQTTMessage mMessage;
     mMessage.payload = (void*)payload;
     mMessage.payloadlen = strlen(payload);
     mMessage.qos = QOS1;
     mMessage.retained = 0;
     mMessage.dup = 0;

#ifdef _MQTT_HANDLE_DEBUG_
     {
         char str[128];
         snprintf(str,128,"MQTT publish call.");
         SKTDebugPrint(LOG_LEVEL_DEBUG, str);
         snprintf(str,128,"topic : %s", topic);
         SKTDebugPrint(LOG_LEVEL_DEBUG, str);
         SKTDebugPrint(LOG_LEVEL_DEBUG, mMessage.payload);
         snprintf(str,128,"payloadlen : %d", mMessage.payloadlen);
         SKTDebugPrint(LOG_LEVEL_DEBUG, str);
         snprintf(str,128,"qos : %d", mMessage.qos);
         SKTDebugPrint(LOG_LEVEL_DEBUG, str);
         snprintf(str,128,"retained : %d", mMessage.retained);
         SKTDebugPrint(LOG_LEVEL_DEBUG, str);
         snprintf(str,128,"dup : %d", mMessage.dup);
         SKTDebugPrint(LOG_LEVEL_DEBUG, str);
     }
#endif

     int rc = SUCCESS;
     if((rc = MQTTPublish(g_pMQTTClient, topic, &mMessage)) != SUCCESS)
     {
         SKTDebugPrint(LOG_LEVEL_ERROR, "MQTT publish Failed.");
     }
     else
     {
         CallMessageDeliveredCallback();
     }

     return rc;
}


int handleMQTTYield(unsigned long timeout_ms)
{
    SKTDebugPrint(LOG_LEVEL_INFO, "handleMQTTYield()");

    int rc = SUCCESS;
    if(g_pMQTTClient != NULL)
    {
        rc = MQTTYield(g_pMQTTClient, timeout_ms);
    }

    return rc;
}

/**
 * @brief is connected
 * @return true : connected <-> false
 */
int handleMQTTIsConnected(void)
{
    SKTDebugPrint(LOG_LEVEL_INFO, "handleMQTTIsConnected()");

    int rc = FAILURE;
    if(g_pMQTTClient != NULL)
    {
        rc = IsNetworkConnected(g_pMQTTClient->ipstack);
    }

    return rc;
}


/**
 * @brief disconnect mqtt
 */
int handleMQTTDisconnect(void)
{
    SKTDebugPrint(LOG_LEVEL_INFO, "handleMQTTDisconnect()");

    int rc = SUCCESS;
    if(g_pMQTTClient != NULL && IsNetworkConnected(g_pMQTTClient->ipstack))
    {
        rc = MQTTDisconnect(g_pMQTTClient);

        if(!g_enableServerCertAuth) { NetworkDisconnect(g_pMQTTClient->ipstack); }
#ifdef IMPORT_TLS_LIB
        else                        { NetworkDisconnectTLS(g_pMQTTClient->ipstack); }
#endif
    }

    CallDisconnectedCallback(rc);

    return rc;
}

/**
 * @brief destroy mqtt
 */
void handleMQTTDestroy(void)
{
    SKTDebugPrint(LOG_LEVEL_INFO, "handleMQTTDestroy()");

    if(g_pMQTTClient != NULL && IsNetworkConnected(g_pMQTTClient->ipstack))
    {
        handleMQTTDisconnect();
    }

    if(gContent) {
        if(gContent->data) {
            free(gContent->data);
            gContent->data = NULL;
        }
        free(gContent);
        gContent = NULL;
    }
    if(g_pMQTTClient != NULL)
    {
        free(g_pMQTTClient->ipstack);
        free(g_pMQTTClient->buf);
        free(g_pMQTTClient->readbuf);
        free(g_pMQTTClient);
        g_pMQTTClient = NULL;
    }
}


