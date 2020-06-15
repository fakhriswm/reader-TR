#include "mqtt_manager/mqtt_manager.h"

#include <ArduinoJson.h>
#include <WiFi.h>
#include "tool/tool.h"
#include "device_code/device_code.h"

#define TOKEN_LENGTH 6
#define PUBS_TOPIC  dev_code.dev_type() + "rs/" + dev_code.dev_id()
#define HB_TOPIC    dev_code.dev_type() + "hb"
#define DATA_TOPIC  dev_code.dev_type() + "rc/" + dev_code.dev_id()
#define HEAD_TOPIC  dev_code.dev_type() + dev_code.dev_id()
#define SUBS_TOPIC  dev_code.dev_type() + dev_code. dev_id() + "/#"

WiFiClient espClient;
PubSubClient client(espClient);
extern dev_code dev_code;

String backend_server = "";
String backend_username = "";
String backend_passwrd = "";
String subs_topic = SUBS_TOPIC;
String pubs_topic = PUBS_TOPIC;
String hb_topic = HB_TOPIC;
uint16_t backend_port = 0;

unsigned long previous = 0;

void mqtt :: mqtt_publish(String topic, String payload){
  client.publish(topic.c_str(),payload.c_str());
  Serial.println("publish data :" + topic + " " + payload);
}

void mqtt_callback(char* topic, byte* payload, unsigned int len){
  extern mqtt mqtt;
  String topic_s = topic;
      
  String payload_s = (char*)payload;
  payload_s = payload_s.substring(0,len);
  
  String pub = PUBS_TOPIC;
 
  if(topic_s.substring(0,9) == HEAD_TOPIC){
    DynamicJsonDocument json_doc(30);
    deserializeJson(json_doc,payload_s);
    String command = json_doc["command"];
    if(command == "allow"){
      // myservo_open();
      // delay(4000);
      // myservo_close();      
    }
    else if(command == "reject"){
      // myservo_close();
    }
    else if(command == "test" ){
      mqtt.mqtt_publish("CH806","akupunyaanjingkecil");
    }
  }  
}

void mqtt :: mqtt_init(){
  Serial.print("connecting to: ");
  Serial.print(backend_server);
  Serial.print(backend_port);
  Serial.println(backend_username);
  client.setServer(backend_server.c_str(), backend_port);
  client.setCallback(mqtt_callback);
}

void mqtt :: mqtt_reconnect(){
  if (!client.connected()) {
    // if(!wifi_status()){
    //   setup_wifi_sta();
    // }
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    const char* set_username = NULL;
    const char* set_password = NULL;
    String dev_id = dev_code.dev_id();
    
    if(backend_username != "" && backend_passwrd != ""){
      set_username = backend_username.c_str();
      set_password = backend_passwrd.c_str();
      }
    if (client.connect(dev_id.c_str(), set_username, set_password)) {
      Serial.println("connected");
      // Subscribe
      Serial.println("subs topic: " + subs_topic);
      client.subscribe(subs_topic.c_str());
    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(1000);
    }
  }
}

boolean mqtt :: mqtt_loop(){
  return client.loop();
}

void mqtt :: handle_newbeacon(String value){
  uint16_t major = parse_string(value,',',0).toInt();
  uint16_t minor = parse_string(value,',',1).toInt();
  String raw_uuid = parse_string(value,',',2);
  String uuid = "";
  // iBeacon_stop();
  // aes_encrypt(raw_uuid,uuid);
  // iBeacon_set(major,minor,1000,uuid);
  // iBeacon_start();
}

void mqtt :: append_beacon(String lastdigit_uuid, uint16_t major, uint16_t minor){
  DynamicJsonDocument json_doc(70);
  json_doc["type"] = "beacon";
  json_doc["uid"] = lastdigit_uuid;
  json_doc["major"] = major;
  json_doc["minor"] = minor;
  String pub_data;
  serializeJson(json_doc,pub_data);
  mqtt_publish(pubs_topic,pub_data);
}

void mqtt :: append_mifare(String uid){
  String payload = "";
  DynamicJsonDocument json_doc(50);
  json_doc["type"] = "mifare";
  json_doc["uid"] = uid;
  String pub_data;
  serializeJson(json_doc,pub_data);
  mqtt_publish(pubs_topic,pub_data);
}
void mqtt :: send_hb(){
  unsigned long current = millis();
  if(current - previous >= 5000){
    previous = current;
    mqtt_publish(hb_topic,dev_code.dev_id());
  }
}


