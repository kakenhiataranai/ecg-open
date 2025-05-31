
#include "fasade_mqtt.h"


fasade_parent_process_mqtt::fasade_parent_process_mqtt(const char* ip, int port, int keepalive, const char* username, const char* password, const char* id, bool clean_session, const char* topic, int quality_of_service, bool retain, int step_size_msec) : mqtt_obj(ip, port, keepalive, username, password, id, clean_session, topic, quality_of_service, retain) {

 this->mqtt_obj.mqtt_publish_initialize();

 this->mqtt_obj.set_step_size_msec(step_size_msec);

}

void fasade_parent_process_mqtt::fasade_parent_process_mqtt_execute(struct shared_memory* shared_memory_pointer) {

 /*                */
 /* MQTT 送信 実行 */
 /*                */
 this->mqtt_obj.mqtt_publish_execute(shared_memory_pointer);

}