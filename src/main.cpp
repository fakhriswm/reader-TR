#include <Arduino.h>

#include "ibeacon_adv/ibeacon_adv.h"
#include "notif/notif.h"
#include "ble_scanning/ble_scanning.h"
#include "mqtt_manager/mqtt_manager.h"
#include "wifi_manager/wifi_manager.h"
#include "mifare_reader/mifare_reader.h"
#include "webserver_handle/webserver_handle.h"
#include "device_code/device_code.h"
#include "myservo/myservo.h"

#define BEACON_UUID "cb10023f-a318-3394-4199-a8730c7c1aec" // UUID 1 128-Bit (may use linux tool uuidgen or random numbers via https://www.uuidgenerator.net/)

#define SERVO_PIN 12
#define INTERVAL 1000

#define REBOOT_TOPIC "CH806"

mqtt mqtt;
notif notif;
flash flash;
wifi_manager wifi_manager;
webserver webserver;
ble_scanning ble;
mifare mifare;
iBeacon ibeacon;
dev_code dev_code;
myservo myservo;

unsigned long prevMillis = 0;
boolean net_state = false;

void setup() {
    notif.notif_init();
    myservo.myservo_init(SERVO_PIN);
    myservo.myservo_close();
    notif.notif_powerup();
    Serial.begin(115200);
    delay(100);
    if(!flash.flash_init()){
        Serial.println("An Error has occurred while mounting SPIFFS");
    }
    flash.config_all();
    wifi_manager.setup_wifi_ap();
    webserver.webserver_start();
    wifi_manager.setup_wifi_sta();
    ble.ble_scan_init();
    mifare.mifare_init();
    ibeacon.iBeacon_init();
    ibeacon.iBeacon_set(111,1387,500,BEACON_UUID);
    ibeacon.iBeacon_start();
    mqtt.mqtt_init();
    mqtt.mqtt_reconnect();
    mqtt.mqtt_publish(REBOOT_TOPIC,dev_code.dev_id());
}

void loop() {
 net_state = true;
 unsigned long current_millis = millis();
 mqtt.send_hb();
 ble.read_ble();
 mifare.mifare_read();
 if (current_millis - prevMillis >= INTERVAL){
    prevMillis = current_millis;
    ble.check_beacon();
    notif.notif_network_connected();
    if(!mqtt.mqtt_loop()){
      notif.notif_network_notconnected();
      mqtt.mqtt_reconnect();
      net_state = false;
    }
  }
}