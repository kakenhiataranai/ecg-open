

#include "sub_to_json.h"
#include <libconfig.h++>


struct Setting{

/*                 */
/* config_xxxx.txt */
/*                 */
 std::string CONFIG_FILENAME;
 std::string JSON_STORE_DIR;
 std::string JSON_FILE_NAME;
 long long JSON_LOCK_USLEEP_MSEC;
 std::string CUI_CHARACTER_STRING;
 std::string IP;
 int PORT;
 int KEEPALIVE;
 std::string USERNAME;
 std::string PASSWORD;
 std::string ID;
 bool CLEAN_SESSION;
 std::string TOPIC;
 int QUALITY_OF_SERVICE;
 int TIMEOUT;
 int MAX_PACKETS;
};


Setting read_config(const char *in_sub){

 Setting setting;
 libconfig::Config cfg;

 try{

  cfg.readFile(in_sub);
  setting.CONFIG_FILENAME = std::string(in_sub);
  cfg.lookupValue("JSON_STORE_DIR", setting.JSON_STORE_DIR);
  cfg.lookupValue("JSON_FILE_NAME", setting.JSON_FILE_NAME);
  cfg.lookupValue("JSON_LOCK_USLEEP_MSC", setting.JSON_LOCK_USLEEP_MSEC);
  cfg.lookupValue("CUI_CHARACTER_STRING", setting.CUI_CHARACTER_STRING);
  cfg.lookupValue("IP", setting.IP);
  cfg.lookupValue("PORT", setting.PORT);
  cfg.lookupValue("KEEPALIVE", setting.KEEPALIVE);
  cfg.lookupValue("USERNAME", setting.USERNAME);
  cfg.lookupValue("PASSWORD", setting.PASSWORD);
  cfg.lookupValue("ID", setting.ID);
  cfg.lookupValue("CLEAN_SESSION", setting.CLEAN_SESSION);
  cfg.lookupValue("TOPIC", setting.TOPIC);  
  cfg.lookupValue("QUALITY_OF_SERVICE", setting.QUALITY_OF_SERVICE);
  cfg.lookupValue("TIMEOUT", setting.TIMEOUT);
  cfg.lookupValue("MAX_PACKETS", setting.MAX_PACKETS);

 }
 catch(const libconfig::FileIOException &fioex){
  std::cout << "No found config file" << std::endl;exit(-1);
 }
 catch(const libconfig::ParseException &pex){
  std::cout << "Parse error" << std::endl;exit(-1);
 }

 return setting;
}



int main(int argc, char* argv[]) {
 if (argc < 2) {
  std::cout << "./out configfile (ex: ./out setting.txt)\n";
  exit(-1);
 }

 Setting setting = read_config(argv[1]);

 std::string json_store_dir = setting.JSON_STORE_DIR;             /* = "/dev/shm/files"  */
 std::string json_file_name = setting.JSON_FILE_NAME;             /* = "xxxxx.json"      */
 long long json_lock_usleep_msec = setting.JSON_LOCK_USLEEP_MSEC; /* = 1                 */
 std::string cui_character_string = setting.CUI_CHARACTER_STRING; /* = "<< Subscriber : Receiving Pseudo HEART ! >>" */
 const char* ip = setting.IP.c_str();                             /* = "150.18.225.136"  */
 int port = setting.PORT;                                         /* = 1883              */
 int keepalive = setting.KEEPALIVE;                               /* = 60                */
 const char* username = setting.USERNAME.c_str();                 /* = "guest"           */
 const char* password = setting.PASSWORD.c_str();                 /* = "password"        */
 const char* id = setting.ID.c_str();                             /* = "pecg_sub"        */
 bool clean_session = setting.CLEAN_SESSION;                      /* = true              */
 const char* topic = setting.TOPIC.c_str();                       /* = "test/sensor/heart rate" */
 int quality_of_service = setting.QUALITY_OF_SERVICE;             /* = 0                 */
 int timeout = setting.TIMEOUT;                                   /* = -1                */
 int max_packets = setting.MAX_PACKETS;                           /* = 1                 */

 class mqtt_subscribe mqtt_subscribe_obj(
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
  max_packets
 );

 std::string json_file_path = json_store_dir + "/" + json_file_name;
 mqtt_subscribe_obj.set_json_file_path(json_file_path);
 mqtt_subscribe_obj.set_json_lock_usleep_msec(json_lock_usleep_msec);
 mqtt_subscribe_obj.set_cui_character_string(cui_character_string);




 /*                                 */
 /* リアルタイムMQTT受信 ループ開始 */
 /*                                 */
 //int cnt = 5;
 //for (int i = 0; i < cnt; i++) {
 // /* 親プロセスのカウンタ更新の検出 */
 // [[maybe_unused]] int loop_res = 0;
 // /* MQTTの受信をループしておく */
 // loop_res = mqtt_subscribe_obj.call_mosquitto_loop();
 //}


 while (true) {
  /* 親プロセスのカウンタ更新の検出 */
  [[maybe_unused]] int loop_res = 0;
  /* MQTTの受信をループしておく */
  loop_res = mqtt_subscribe_obj.call_mosquitto_loop();
 }



 return 0;
}
