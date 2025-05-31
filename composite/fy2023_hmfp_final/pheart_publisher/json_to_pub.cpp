

#include "json_to_pub.h"


mqtt_publish::mqtt_publish(
 const char* ip, 
 int port, 
 int keepalive, 
 const char* username, 
 const char* password, 
 const char* id, 
 bool clean_session, 
 const char* topic, 
 int quality_of_service, 
 bool retain) {
 this->ip = ip;
 this->port = port;
 this->keepalive = keepalive;
 this->username = username;
 this->password = password;
 this->id = id;
 this->clean_session = clean_session;
 this->topic = topic; 
 this->quality_of_service = quality_of_service;
 this->retain = retain;

 mosquitto_lib_init();

 this->mosq_pub = mosquitto_new(id, clean_session, NULL);
 if (!mosq_pub) {
  std::cout << "Error: Failed to create mosq_pub" << std::endl;
  mosquitto_lib_cleanup();
  exit(0);
 }

 mosquitto_username_pw_set(mosq_pub, username, password);
 mosquitto_connect(mosq_pub, ip, port, keepalive);

 this->swap = true;
}

mqtt_publish::~mqtt_publish() {
 mosquitto_destroy(mosq_pub);
 mosquitto_lib_cleanup();
}


int mqtt_publish::mqtt_publish_execute() {

 
 /* UNIX時間を取得 */
 auto now = std::chrono::system_clock::now();
 auto duration = now.time_since_epoch();
 long long actual_msec_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

 /* JSONオブジェクトを作成 */
 nlohmann::json obj;
 if (swap) {

  obj = {
 {"msec since epoc", actual_msec_since_epoch},
 {"bpm", 63.7543},
 {"normal", true}
  };

  swap = false;

 }
 else {

  obj = {
 {"msec since epoc", actual_msec_since_epoch},
 {"bpm", 20.7543},
 {"normal", true}
 };


  swap = true;
 }


 /* バフリッシュ */
 std::string obj_str = obj.dump();
 const char* payload = obj_str.c_str();
 mosquitto_publish(mosq_pub, NULL, topic, strlen(payload), payload, quality_of_service, retain);

 /* CUIに表示 */
 std::cout << payload << std::endl;

 return EXIT_SUCCESS;
}
