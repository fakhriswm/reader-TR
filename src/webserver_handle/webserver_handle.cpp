#include "webserver_handle.h"
#include "mqtt_manager/mqtt_manager.h"
#include "wifi_manager/wifi_manager.h"
#include "ble_scanning/ble_scanning.h"
#include "mifare_reader/mifare_reader.h"

AsyncWebServer myserver(80);
extern flash flash;

extern String sta_ssid;
extern String sta_passwrd;
extern String ap_ssid;
extern String ap_passwrd;
extern String backend_server;
extern String backend_username;
extern String backend_passwrd;
extern uint16_t backend_port;
extern float proximity;
extern int ble_timeout;
extern uint8_t detect_counter;
extern String master_key;

extern bool net_state;
extern void reset_esp();

String processor(const String& var){
  if(var == "STA_SSID"){
    return sta_ssid.c_str();
  }
  else if(var == "STA_PASSWORD"){
    return sta_passwrd.c_str();
  }
  else if(var == "AP_SSID"){
    return ap_ssid.c_str();
  }
  else if(var == "AP_PASSWORD"){
    return ap_passwrd.c_str();
  }
  else if(var == "MQTT_BROKER"){
    return backend_server.c_str();
  }
  else if(var == "MQTT_USERNAME"){
    return backend_username.c_str();
  }
  else if(var == "MQTT_PASSWORD"){
    return backend_passwrd.c_str();
  }
  else if(var == "MQTT_PORT"){
    return (String)backend_port;
  }
  else if(var == "PROXIMITY"){
    return (String)proximity;
  }
  else if(var == "BLE_TIMEOUT"){
    return (String)ble_timeout;
  }
  else if(var == "DETECT_COUNTER"){
    return (String)detect_counter;
  }
  else if(var == "MASTER_KEY"){
    return master_key.c_str();
  }
  else if(var == "NETSTATE"){
    if(net_state){
      return "CONNECTED";
    }
    else{
      return "NOT CONNECTED";
    }
  }
  return String();
}

static void set_network_sta(AsyncWebServerRequest *request){
  if(request->arg("ssid") != NULL && request->arg("password") != NULL){
    sta_ssid = request->arg("ssid");
    sta_passwrd = request->arg("password");
    flash.set_sta(sta_ssid.c_str(),sta_passwrd.c_str());
    request->send(200, "text/html", "Setting station network success, please return home page and reboot soon! <br><a href=\"/home\">Return to Home Page</a>");
  }
  else{
    request->send(200, "text/html", "please enter SSID & Password completely");
  }
}

static void set_network_ap(AsyncWebServerRequest *request){
  if(request->arg("ssid") != NULL && request->arg("password") != NULL){
    ap_ssid = request->arg("ssid");
    ap_passwrd = request->arg("password");
    flash.set_ap(ap_ssid.c_str(),ap_passwrd.c_str());
    request->send(200, "text/html", "Setting access point network success, please return home page and reboot soon! <br><a href=\"/home\">Return to Home Page</a>");
  }
  else{
    request->send(200, "text/html", "please enter SSID & Password completely");
  }
}

static void set_backendserver(AsyncWebServerRequest *request){
  if(request->arg("mqtt_broker")&& request->arg("mqtt_username")&& request->arg("mqtt_password")&& request->arg("mqtt_port")){
    backend_server = request->arg("mqtt_broker");
    backend_username = request->arg("mqtt_username");
    backend_passwrd = request->arg("mqtt_password");
    backend_port = request->arg("mqtt_port").toInt();
    flash.set_backend(backend_server.c_str(), backend_username.c_str(), backend_passwrd.c_str(), backend_port);
    request->send(200, "text/html", "Setting Backend success, please return home page and reboot soon! <br><a href=\"/home\">Return to Home Page</a>");
  }
  else{
    request->send(200, "text/html", "please enter mqtt param completely");
  }
}

static void set_scanning_param(AsyncWebServerRequest *request){
  if(request->arg("proximity")&& request->arg("ble_timeout")&& request->arg("detect_counter")&& request->arg("master_key")){
    char char1[8]; 
    String buff = request->arg("proximity");
    buff.toCharArray(char1,buff.length()+1);
    proximity = atof(char1);
    ble_timeout = request->arg("ble_timeout").toInt();
    detect_counter = request->arg("detect_counter").toInt();
    master_key = request->arg("master_key");
    flash.set_scanning( proximity, ble_timeout, detect_counter, master_key);
    request->send(200, "text/html", "Setting scanning param success, please return home page and reboot soon! <br><a href=\"/home\">Return to Home Page</a>");
  }
  else{
    request->send(200, "text/html", "please enter scanning parameter completely"); 
  }
}

static void homepage(AsyncWebServerRequest *request){
  request->send_P(200, "text/html", index_html, processor);
}

static void login(AsyncWebServerRequest *request){
  request->send_P(200, "text/html", login_html);
}

static void afterlogin(AsyncWebServerRequest *request){
  String username = request->arg("username");
  String password = request->arg("password");
  if(username == "admin" && password == "admin"){
   request->send_P(200, "text/html", index_html, processor);
  }
  else{
    request->send(200, "text/html", "username and password invalid! <br><a href=\"/\">Return to Landing Page</a>");
  }

}

static void reboot(AsyncWebServerRequest *request){
  request->send(200, "text/html", "Hardware reboot, please wait for a while and reconnect again!");
  ESP.restart();
}

void webserver :: webserver_start(){
  myserver.on("/", HTTP_GET, login);
  myserver.on("/home", HTTP_GET, homepage);
  myserver.on("/config/network_sta", HTTP_POST,set_network_sta);
  myserver.on("/config/network_ap", HTTP_POST,set_network_ap); 
  myserver.on("/config/backend", HTTP_POST,set_backendserver); 
  myserver.on("/config/scanning", HTTP_POST,set_scanning_param);
  myserver.on("/reboot", HTTP_GET,reboot);
  myserver.on("/login", HTTP_POST, afterlogin);
  myserver.begin();
}
