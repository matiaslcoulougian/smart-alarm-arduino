#include <WiFi.h>
#include <PubSubClient.h>
#include <string.h>
#include "mqtt_sub.h"

const char* mqttUser = "/";
const char* mqttPassword = "/";
const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;

static char client_name[20];

int activation_times[100];
int activation_times_length = 0;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

//message recived from server
static void callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    char activation_time_str[80] = "";
    int activation_time_str_index = 0;
    char message_char;
    int index = 0;
    memset(activation_times, 0, sizeof activation_times);
    for (int i = 0; i < sizeof(activation_time_str); i++) {
        message_char = (char) payload[i];
        if (message_char == ',') {
            Serial.println(activation_time_str);
            activation_times[index] = std::stoi(activation_time_str);
            index += 1;
            memset(activation_time_str, 0, sizeof activation_time_str);
            activation_time_str_index = 0;
        }
        else {
            activation_time_str[activation_time_str_index] = message_char;
            activation_time_str_index += 1;
        }
    }
    activation_times_length = sizeof(activation_times);
}





static void do_subscribe( const char *ptopic ) {
    mqttClient.subscribe( ptopic );
}


static void do_connect(void) {
    Serial.println("DO CONNECT");
    while (!mqttClient.connected())
    {
        Serial.println("Connecting to MQTT...");

        if (mqttClient.connect( client_name, mqttUser, mqttPassword))
            Serial.printf( "client %s connected\n\r", client_name );
        else
        {
            Serial.print("MQTT connection: failed with state ");
            Serial.println(mqttClient.state());
            delay(5000);
        }
    }
    do_subscribe("smart-alarm");
    Serial.printf( "Connected to %s\n\r", mqttServer );

}

//  init_mqtt: connection to an MQTT server

void init_mqtt(void) {
    mqttClient.setServer(mqttServer, mqttPort);
    mqttClient.setCallback(callback);
    do_connect();
}




//  mqtt_comms: manage communications with MQTT server

void mqtt_comms(void){
    if( !mqttClient.connected() )
        do_connect();
    mqttClient.loop();
}