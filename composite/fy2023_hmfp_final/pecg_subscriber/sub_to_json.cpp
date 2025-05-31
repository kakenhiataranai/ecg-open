

#include "sub_to_json.h"


mqtt_subscribe::mqtt_subscribe(
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

 mosquitto_lib_init();

 this->mosq_sub
  = mosquitto_new(id, clean_session, this);
 if (!mosq_sub) {
  std::cout << "Failed to create mosq_sub" << std::endl;
  mosquitto_lib_cleanup();
  exit(0);
 }

 mosquitto_username_pw_set(mosq_sub, username, password); 

 /* 受信した時に自動で呼び出す関数を指定 */
 mosquitto_message_callback_set(mosq_sub, &mqtt_subscribe::on_message);

 ///* デバッグ用 */
 //mosquitto_log_callback_set(mosq_sub, &mqtt_subscribe::log_callback);

 mosquitto_connect(mosq_sub, ip, port, keepalive);
 mosquitto_subscribe(mosq_sub, NULL, topic, quality_of_service);  
 mosquitto_loop(mosq_sub, timeout, max_packets); /* ここにも書いておく */
}

mqtt_subscribe::~mqtt_subscribe() {
 mosquitto_destroy(mosq_sub);
 mosquitto_lib_cleanup();
}


/* 成功: 0 / 失敗 : 1 */
int mqtt_subscribe::call_mosquitto_loop() {
 int res = 1;
 res = mosquitto_loop(mosq_sub, timeout, max_packets);
 return res;
}


///* デバッグ用 */
//void mqtt_subscribe::log_callback(struct mosquitto* mosq_sub, void* userdata, int level, const char* str) {
// /* ログレベルに応じてメッセージを出力 */
// std::cout << "[LOG] " << str << std::endl;
//}


void mqtt_subscribe::on_message(struct mosquitto* mosq_sub, void* userdata, const struct mosquitto_message* msg) {
 // obj ではコンパイルエラーなので userdata にした

 /* メンバアクセス用のオブジェクトを準備 */
 mqtt_subscribe* this_class_obj = static_cast<mqtt_subscribe*>(userdata);

 if (msg->payloadlen > 0) {

  std::string message((const char*)msg->payload);

  /* CUIに表示 */
  std::cout << this_class_obj->cui_character_string << "  " << message << std::endl;

  /* json ではなく自動ソートしない ordered_json を使用 */
  nlohmann::ordered_json json_obj = nlohmann::ordered_json::parse(message);
  this_class_obj->overwrite_json_to_file_with_lock(json_obj);
 }
 else { std::cout << "Message is null..." << std::endl; }
}


void mqtt_subscribe::set_json_file_path(std::string json_file_path) {
 this->json_file_path = json_file_path;
}


void mqtt_subscribe::set_json_lock_usleep_msec(long long json_lock_usleep_msec) {
 this->json_lock_usleep_msec = json_lock_usleep_msec;
}


void mqtt_subscribe::set_cui_character_string(std::string cui_character_string) {
 this->cui_character_string = cui_character_string;
}


void mqtt_subscribe::append_json_to_file(const nlohmann::ordered_json & json_obj) {
 std::ofstream file_stream(this->json_file_path, std::ios::app);
 if (!file_stream.is_open()) {
  std::cerr << "Error: Unable to open file for writing." << std::endl;
  return;
 }

 /* 追記モードで書き込む */
 file_stream << json_obj << std::endl;
 file_stream.close();
}


void mqtt_subscribe::overwrite_json_to_file_with_lock(const nlohmann::ordered_json& json_obj) {
 
 std::ofstream file(this->json_file_path);

 /* ファイルディスクリプタを取得 */
 /* O_WRONLY 書き込み専用で開く                      */
 /* O_CREAT ファイルが存在しない場合にファイルを作成 */
 /* O_TRUNC ファイルが存在する場合にその内容を切り捨て */
 int fd = open(this->json_file_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
 if (fd == -1) {
  std::cerr << "Error: Failed to open file." << std::endl;
  return;
 }


 /* ロックの設定 */
 struct flock fl;
 fl.l_type = F_WRLCK;    // 書き込みロック
 fl.l_whence = SEEK_SET; // ファイルの先頭から相対的な位置
 fl.l_start = 0;         // ロックの開始位置
 fl.l_len = 0;           // ロックの長さ
 fl.l_pid = getpid();    // ロックを設定したプロセスのID

 
 /* 排他的なロックを取得するまで待機 */
 while (fcntl(fd, F_SETLK, &fl) == -1) {
  /* ロックが他のプロセスによって使用中の場合は待機 */
  if (errno == EACCES || errno == EAGAIN) {   
   std::cerr << "Error: Lock is held by another process. Retrying..." << std::endl;
   std::this_thread::sleep_for(std::chrono::milliseconds(this->json_lock_usleep_msec));
  }
  /* それ以外の場合は関数を終了 */
  else {
   std::cerr << "Error: Failed to acquire lock on file." << std::endl;
   close(fd);
   return;
  }
 }

 /* ファイルに書き込む */
 file << json_obj << std::endl;

 /* 書き込みが終わったらロックを解除 */
 fl.l_type = F_UNLCK;
 if (fcntl(fd, F_SETLK, &fl) == -1) {
  std::cerr << "Error: Failed to release lock on file." << std::endl;
 }

 close(fd);
}