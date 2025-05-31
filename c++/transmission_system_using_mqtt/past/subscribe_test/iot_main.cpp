/*                                                                           */
/* MQTT受信のテストプログラム                                                */
/*                                                                           */

#include "iot_common.h"
#include "fasade_mqtt_subscribe_ecg.h"
#include <libconfig.h++>


struct Setting{

/*                           */
/* sub_config.txt            */
/*                           */
 std::string CONFIG_FILENAME_SUB;
 std::string IP_SUB;
 int PORT_SUB;
 int KEEPALIVE_SUB;
 std::string USERNAME_SUB;
 std::string PASSWORD_SUB;
 std::string ID_SUB;
 bool CLEAN_SESSION_SUB;
 std::string TOPIC_SUB;
 int QUALITY_OF_SERVICE_SUB;
 int TIMEOUT_SUB;
 int MAX_PACKETS_SUB;

};

Setting read_config(const char *in_sub){

 Setting setting;
 libconfig::Config cfg;

 try{

  cfg.readFile(in_sub);
  setting.CONFIG_FILENAME_SUB = std::string(in_sub);  
  cfg.lookupValue("IP", setting.IP_SUB);
  cfg.lookupValue("PORT", setting.PORT_SUB);
  cfg.lookupValue("KEEPALIVE", setting.KEEPALIVE_SUB);
  cfg.lookupValue("USERNAME", setting.USERNAME_SUB);
  cfg.lookupValue("PASSWORD", setting.PASSWORD_SUB);
  cfg.lookupValue("ID", setting.ID_SUB);
  cfg.lookupValue("CLEAN_SESSION", setting.CLEAN_SESSION_SUB);
  cfg.lookupValue("TOPIC", setting.TOPIC_SUB);  
  cfg.lookupValue("QUALITY_OF_SERVICE", setting.QUALITY_OF_SERVICE_SUB);
  cfg.lookupValue("TIMEOUT", setting.TIMEOUT_SUB);
  cfg.lookupValue("MAX_PACKETS", setting.MAX_PACKETS_SUB);

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


 /* 受信と取得に必要な初期化 */
 const char* ip = setting.IP_SUB.c_str();                 /* = "150.82.92.210"     */
 int port = setting.PORT_SUB;                             /* = 1883                */
 int keepalive = setting.KEEPALIVE_SUB;                   /* = 60                  */
 const char* username = setting.USERNAME_SUB.c_str();     /* = "uchiumi"           */
 const char* password = setting.PASSWORD_SUB.c_str();     /* = "3214neknek"        */
 const char* id = setting.ID_SUB.c_str();                 /* = "iot_prototype_sub" */
 bool clean_session = setting.CLEAN_SESSION_SUB;          /* = true                */
 const char* topic = setting.TOPIC_SUB.c_str();           /* = "pseudo_ecg_topic"  */
 int quality_of_service = setting.QUALITY_OF_SERVICE_SUB; /* = 0                   */
 int timeout = setting.TIMEOUT_SUB;                       /* = -1                  */
 int max_packets = setting.MAX_PACKETS_SUB;               /* = 1                   */

 class fasade_mqtt_subscribe_ecg fasade_mqtt_subscribe_ecg_obj(
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

 /*                                 */
 /* リアルタイムMQTT受信 ループ開始 */
 /*                                 */
 while (true) {

  // debug
  std::cout << "ここはきてるmain" << std::endl;

  /* 親プロセスのカウンタ更新の検出 */
  [[maybe_unused]] int loop_res = 0;
   /* MQTTの受信をループしておく */
   loop_res = fasade_mqtt_subscribe_ecg_obj.fasade_call_mosquitto_loop();
 }

 return 0;
}
