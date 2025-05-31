
#include "mqtt.h"


mqtt::mqtt(const char* ip, int port, int keepalive, const char* username, const char* password, const char* id, bool clean_session, const char* topic, int quality_of_service, bool retain) {
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
}

mqtt::~mqtt() {
 mosquitto_destroy(mosq_pub);
 mosquitto_lib_cleanup();
}


void mqtt::mqtt_publish_initialize() {

 mosquitto_lib_init();

 this->mosq_pub = mosquitto_new(id, clean_session, NULL);
 if (!mosq_pub) {
  std::cout << "Failed to create mosq_pub" << std::endl;
  mosquitto_lib_cleanup();
  exit(0);
 }

 /* ユーザ名とパスワードを設定 */
 mosquitto_username_pw_set(mosq_pub, username, password);

 /* サーバ接続 */
 mosquitto_connect(mosq_pub, ip, port, keepalive);
}


void mqtt::mqtt_publish_execute(struct shared_memory* shared_memory_pointer) {

 int one_cycle_num = shared_memory_pointer->child_process_ecg_row_num;
 int channel_num = shared_memory_pointer->child_process_ecg_column_num;

 /* タイムスタンプ生成 */
 auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

 for (int step = 0; step < one_cycle_num; step++) {

  std::string message = "{";

  /* 心電図データ追加 */
  for (int chan_cnt = 0; chan_cnt < channel_num; chan_cnt++) {
   double value = shared_memory_pointer->child_process_ecg_array[step][chan_cnt];
   message += "\"channel[" + std::to_string(chan_cnt) + "]\":" + std::to_string(value) + ",";
  }

  /* タイムスタンプ追加 */
  std::string msec_since_epoch = std::to_string(msec);
  message += "\"actual msec since epoch\":" + msec_since_epoch + "}";

  /* バフリッシュ */
  const char* payload = message.c_str();
  mosquitto_publish(mosq_pub, NULL, topic, strlen(payload), payload, quality_of_service, retain);
  std::cout << "[[ MQTT Publisher : BIOSIGNAL GENERATOR is SENDING pseudo ECG ! ]]  " << payload << std::endl;

  /* タイムスタンプを進める */
  msec += step_size_msec;
 }

}

void mqtt::set_step_size_msec(int step_size_msec) {

 this->step_size_msec = step_size_msec;

}