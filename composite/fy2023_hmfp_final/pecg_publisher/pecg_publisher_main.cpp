

#include "json_to_pub.h"
#include <libconfig.h++>



struct Setting {

 /*                 */
 /* config_xxxx.txt */
 /*                 */
 std::string CONFIG_FILENAME; 
 std::string JSON_STORE_DIR;
 std::string JSON_FILE_NAME;
 std::string JSON_KEY_TIME;
 int JSON_SAMPLING_MSEC;
 std::string CUI_CHARACTER_STRING;
 int USLEEP_SHORTLY_NSEC;
 std::string IP;
 int PORT;
 int KEEPALIVE;
 std::string USERNAME;
 std::string PASSWORD;
 std::string ID;
 bool CLEAN_SESSION;
 std::string TOPIC;
 int QUALITY_OF_SERVICE;
 bool RETAIN;
};


Setting read_config(const char* in_pub) {

 Setting setting;
 libconfig::Config cfg;

 try {

  cfg.readFile(in_pub);
  setting.CONFIG_FILENAME = std::string(in_pub);
  cfg.lookupValue("JSON_STORE_DIR", setting.JSON_STORE_DIR);
  cfg.lookupValue("JSON_FILE_NAME", setting.JSON_FILE_NAME);
  cfg.lookupValue("JSON_SAMPLING_MSEC", setting.JSON_SAMPLING_MSEC);
  cfg.lookupValue("JSON_KEY_TIME", setting.JSON_KEY_TIME);
  cfg.lookupValue("CUI_CHARACTER_STRING", setting.CUI_CHARACTER_STRING);
  cfg.lookupValue("USLEEP_SHORTLY_NSEC", setting.USLEEP_SHORTLY_NSEC);
  cfg.lookupValue("IP", setting.IP);
  cfg.lookupValue("PORT", setting.PORT);
  cfg.lookupValue("KEEPALIVE", setting.KEEPALIVE);
  cfg.lookupValue("USERNAME", setting.USERNAME);
  cfg.lookupValue("PASSWORD", setting.PASSWORD);
  cfg.lookupValue("ID", setting.ID);
  cfg.lookupValue("CLEAN_SESSION", setting.CLEAN_SESSION);
  cfg.lookupValue("TOPIC", setting.TOPIC);
  cfg.lookupValue("QUALITY_OF_SERVICE", setting.QUALITY_OF_SERVICE);
  cfg.lookupValue("RETAIN", setting.RETAIN);

 }
 catch (const libconfig::FileIOException& fioex) {
  std::cout << "No found config file" << std::endl; exit(-1);
 }
 catch (const libconfig::ParseException& pex) {
  std::cout << "Parse error" << std::endl; exit(-1);
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
 int json_sampling_msec = setting.JSON_SAMPLING_MSEC;             /* = 2                 */
 std::string json_key_time = setting.JSON_KEY_TIME;               /* = "ms"              */
 std::string cui_character_string = setting.CUI_CHARACTER_STRING; /* = "[[ MQTT Publisher : Sending Pseudo ECG ! ]]" */
 int usleep_shortly_nsec = setting.USLEEP_SHORTLY_NSEC;           /* = 95000             */
 const char* ip = setting.IP.c_str();                             /* = "150.18.225.136"  */
 int port = setting.PORT;                                         /* = 1883              */
 int keepalive = setting.KEEPALIVE;                               /* = 60                */
 const char* username = setting.USERNAME.c_str();                 /* = "guest"           */
 const char* password = setting.PASSWORD.c_str();                 /* = "password"        */
 const char* id = setting.ID.c_str();                             /* = "pecg_pub"        */
 bool clean_session = setting.CLEAN_SESSION;                      /* = true              */
 const char* topic = setting.TOPIC.c_str();                       /* = "pseudo/ecg"      */
 int quality_of_service = setting.QUALITY_OF_SERVICE;             /* = 0                 */
 bool retain = setting.RETAIN;                                    /* = false             */

 class mqtt_publish mqtt_publish_obj(
  ip,
  port,
  keepalive,
  username,
  password,
  id,
  clean_session,
  topic,
  quality_of_service,
  retain
 );

 std::string json_file_path = json_store_dir + "/" + json_file_name;
 mqtt_publish_obj.set_json_sampling_msec(json_sampling_msec);
 mqtt_publish_obj.set_json_file_path(json_file_path);
 mqtt_publish_obj.set_json_key_time(json_key_time);
 mqtt_publish_obj.set_cui_character_string(cui_character_string);
 mqtt_publish_obj.set_usleep_shortly_nsec(usleep_shortly_nsec);


/*               */
/* MQTT送信 開始 */
/*               */
 [[maybe_unused]] int loop_res = 0;
 loop_res = mqtt_publish_obj.mqtt_publish_execute();

 return 0;
}