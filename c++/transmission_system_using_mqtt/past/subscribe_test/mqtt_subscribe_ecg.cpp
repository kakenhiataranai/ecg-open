
#include "iot_common.h"
#include "mqtt_subscribe_ecg.h"



mqtt_subscribe_ecg::mqtt_subscribe_ecg(
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
 int max_packets) {
 this->ip = ip;
 this->port = port;
 this->keepalive = keepalive;
 this->username = username;
 this->password = password;
 this->id = id;
 this->clean_session = clean_session;
 this->topic = topic; 
 this->quality_of_service = quality_of_service;
 this->timeout = timeout;
 this->max_packets = max_packets;
}

mqtt_subscribe_ecg::~mqtt_subscribe_ecg() {
 mosquitto_destroy(mosq_sub);
 mosquitto_lib_cleanup();
}

void mqtt_subscribe_ecg::mqtt_subscribe_initialize() {

 mosquitto_lib_init();

 this->mosq_sub 
  = mosquitto_new(id, clean_session, this); // 第三引数はthisポインタ
 if (!mosq_sub) {
  std::cout << "Failed to create mosq_sub" << std::endl;
  mosquitto_lib_cleanup();
  exit(0);
 }

 /* ユーザ名とパスワードを設定 */
 mosquitto_username_pw_set(mosq_sub, username, password);

 /* 受信した時に自動で呼び出す関数を指定 */
 mosquitto_message_callback_set(mosq_sub, &mqtt_subscribe_ecg::on_message);

 /* サーバ接続 */
 mosquitto_connect(mosq_sub, ip, port, keepalive);

 /* 購読登録 */
 mosquitto_subscribe(mosq_sub, NULL, topic, quality_of_service);

 /* ここにも書いておく */
 mosquitto_loop(mosq_sub, timeout, max_packets);

 // debug
 std::cout << "ここはきてるsub" << std::endl;

}


/* 成功: 0 / 失敗 : 1 */
int mqtt_subscribe_ecg::call_mosquitto_loop() {
 int res = 1;
 res = mosquitto_loop(mosq_sub, timeout, max_packets);
 return res;
}


void mqtt_subscribe_ecg::on_message(struct mosquitto* mosq_sub, void* userdata, const struct mosquitto_message* msg) { // obj ではコンパイルエラーなので userdata にした

 /* メンバアクセス用のオブジェクト */
 //mqtt_subscribe_ecg* this_class_obj = static_cast<mqtt_subscribe_ecg*>(userdata);

 if (msg->payloadlen > 0) {

  std::string message((const char*)msg->payload);
  //std::cout << "[[ MQTT Subscriber : IOT DEVICE is RECEIVING pseudo ECG from GENERATOR ! ]]  " << message << std::endl;

  std::cout << "メッセージ受信中........." << message << std::endl;


 }
 else { std::cout << "Message is null..." << std::endl; }

}
