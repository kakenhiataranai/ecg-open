
#include "fasade_mqtt_subscribe_ecg.h"



fasade_mqtt_subscribe_ecg::fasade_mqtt_subscribe_ecg(
 const char* ip, 
 int port, 
 int keepalive, 
 const char* username, 
 const char* password, 
 const char* id, 
 bool clean_session, 
 const char* topic, 
 int quality_of_service, 
 int timeout, 
 int max_packets) : 

 mqtt_subscribe_ecg_obj(
  ip, 
  port, 
  keepalive, 
  username, 
  password, 
  id, 
  clean_session, 
  topic, 
  quality_of_service, 
  timeout, 
  max_packets ){

 this->mqtt_subscribe_ecg_obj.mqtt_subscribe_initialize();

}
 


int fasade_mqtt_subscribe_ecg::fasade_call_mosquitto_loop() {
 int res = 1;
 res = this->mqtt_subscribe_ecg_obj.call_mosquitto_loop();
 return res;
}