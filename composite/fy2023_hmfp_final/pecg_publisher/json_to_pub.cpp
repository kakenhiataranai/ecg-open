

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
}

mqtt_publish::~mqtt_publish() {
 mosquitto_destroy(mosq_pub);
 mosquitto_lib_cleanup();
}


int mqtt_publish::mqtt_publish_execute() {

 std::ifstream file(this->json_file_path);

 if (!file.is_open()) {
  std::cerr << "Error: Unable to open file." << std::endl;
  return 1;
 }

 /* .json に格納されているJSONオブジェクトを上から順にパブリッシュする */
 std::string line;
 while (std::getline(file, line)) {

  auto start_time = std::chrono::high_resolution_clock::now();

  try {

   /* パースするとJSONオブジェクトのキーの順序は保存されない */
   nlohmann::json obj = nlohmann::json::parse(line);

   /* UNIX時間を取得 */
   auto now = std::chrono::system_clock::now();
   auto duration = now.time_since_epoch();
   long long actual_msec_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

   /* JSONのタイムスタンプの値を上書き */
   obj[this->json_key_time] = actual_msec_since_epoch;

   /* バフリッシュ */
   std::string obj_str = obj.dump();
   const char* payload = obj_str.c_str();
   mosquitto_publish(mosq_pub, NULL, topic, strlen(payload), payload, quality_of_service, retain);

   /* CUIに表示 */
   //std::cout << this->cui_character_string << "  " << payload << std::endl;
  }
  catch (nlohmann::json::parse_error& e) {
   std::cerr << "Error: Unable to parse JSON " << e.what() << std::endl;
   return EXIT_FAILURE;
  }

  auto end_time = std::chrono::high_resolution_clock::now();
  auto elapsed_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
  std::this_thread::sleep_for(
   std::chrono::nanoseconds(
	1000000 * this->json_sampling_msec) - elapsed_time - std::chrono::nanoseconds(this->usleep_shortly_nsec) // 1ナノ秒は 1e-6ミリ秒 
  ); 

 }

 file.close();

 /* .json を削除 */
 if (std::remove(this->json_file_path.c_str()) != 0) {
  std::cerr << "Error: Unable to delete file" << std::endl;
  return EXIT_FAILURE;
 }

 return EXIT_SUCCESS;
}


void mqtt_publish::set_json_sampling_msec(int json_sampling_msec) {
 this->json_sampling_msec = json_sampling_msec;
}

void mqtt_publish::set_json_file_path(std::string json_file_path) {
 this->json_file_path = json_file_path;
}

void mqtt_publish::set_json_key_time(std::string json_key_time) {
 this->json_key_time = json_key_time;
}

void mqtt_publish::set_cui_character_string(std::string cui_character_string) {
 this->cui_character_string = cui_character_string;
}

void mqtt_publish::set_usleep_shortly_nsec(int usleep_shortly_nsec) {
 this->usleep_shortly_nsec = usleep_shortly_nsec;
}