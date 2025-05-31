
/*                                        */
/* 擬似的生体信号生成器のメインプログラム */
/*                                        */
/* 通信プロトコルは MQTT を使用           */
/*                                        */
/* (子プロセス)                           */
/* 擬似的生体信号データを生成             */
/* 生成したデータを共有メモリに載せる     */
/*                                        */
/* (親プロセス)                           */
/* 共有メモリ上のデータを受け取る         */
/* 受け取ったデータをパブリッシュする     */
/*                                        */


#include "generator_common.h"
#include "ecg_model_parameter.h"
#include "fasade_mqtt.h"
#include "fasade_generate_12_lead_ecg.h"

#include <libconfig.h++>


#define PI 3.1415926535897932384626433832795028841971

#define CHILD_PROCESS_NUM 1         /* 子プロセスの数       */
#define ECG_CHANNEL_NUM 12          /* 12誘導               */
#define MCSHARRY_VARIABLE_NUM 4     /* x, y, z, omega の4つ */
#define VARIABLE_NUM 3              /* x, y, z の3つ */
#define DISTINCT_POINT_NUM 5        /* P, Q, R, S, T の5つ  */
//#define FREQUENCY_MODAL_NUM 2


struct Setting{

 /*                 */
 /* mqtt_config.txt */
 /*                 */
 std::string CONFIG_FILENAME_MQTT;

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

 /*                      */
 /* biosignal_config.txt */
 /*                      */
 std::string CONFIG_FILENAME_BIOSIGNAL;

 int STEP_SIZE_MSEC;
 double ECG_STEP_SIZE;
 int ECG_ONE_CYCLE_STEP_NUM;
 std::vector<std::vector<double>> THETA;
 std::vector<std::vector<double>> A;
 std::vector<std::vector<double>> B;
 std::vector<std::vector<double>> ALPHA;
 double X_INITIAL_VALUE;
 double Y_INITIAL_VALUE;
 double Z_INITIAL_VALUE;
 double OMEGA_INITIAL_VALUE;
 int OMEGA_T_SIZE; 
 double Z_SCALAR;
 double LARGE_A;
 double F_2;
 int ECG_STEP_NUM_PER_SEC;
};

Setting read_config(const char *in_mqtt, const char* in_biosignal){
 
 Setting setting;
 libconfig::Config cfg;

 int ecg_channel_num = ECG_CHANNEL_NUM;
 int distinct_point_num = DISTINCT_POINT_NUM; 
 setting.THETA.resize(ecg_channel_num, std::vector<double>(distinct_point_num, 0));
 setting.A.resize(ecg_channel_num, std::vector<double>(distinct_point_num, 0));
 setting.B.resize(ecg_channel_num, std::vector<double>(distinct_point_num, 0));
 setting.ALPHA.resize(ecg_channel_num, std::vector<double>(distinct_point_num, 0));

 try{

  cfg.readFile(in_mqtt);
  setting.CONFIG_FILENAME_MQTT = std::string(in_mqtt);
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

  cfg.readFile(in_biosignal);
  setting.CONFIG_FILENAME_BIOSIGNAL = std::string(in_biosignal);
  cfg.lookupValue("STEP_SIZE_MSEC", setting.STEP_SIZE_MSEC);
  cfg.lookupValue("ECG_STEP_SIZE", setting.ECG_STEP_SIZE);
  cfg.lookupValue("ECG_ONE_CYCLE_STEP_NUM", setting.ECG_ONE_CYCLE_STEP_NUM);
  cfg.lookupValue("A_0_0", setting.A[0][0]);
  cfg.lookupValue("A_0_1", setting.A[0][1]);
  cfg.lookupValue("A_0_2", setting.A[0][2]);
  cfg.lookupValue("A_0_3", setting.A[0][3]);
  cfg.lookupValue("A_0_4", setting.A[0][4]);
  cfg.lookupValue("A_1_0", setting.A[1][0]);
  cfg.lookupValue("A_1_1", setting.A[1][1]);
  cfg.lookupValue("A_1_2", setting.A[1][2]);
  cfg.lookupValue("A_1_3", setting.A[1][3]);
  cfg.lookupValue("A_1_4", setting.A[1][4]);
  cfg.lookupValue("A_2_0", setting.A[2][0]);
  cfg.lookupValue("A_2_1", setting.A[2][1]);
  cfg.lookupValue("A_2_2", setting.A[2][2]);
  cfg.lookupValue("A_2_3", setting.A[2][3]);
  cfg.lookupValue("A_2_4", setting.A[2][4]);
  cfg.lookupValue("A_3_0", setting.A[3][0]);
  cfg.lookupValue("A_3_1", setting.A[3][1]);
  cfg.lookupValue("A_3_2", setting.A[3][2]);
  cfg.lookupValue("A_3_3", setting.A[3][3]);
  cfg.lookupValue("A_3_4", setting.A[3][4]);
  cfg.lookupValue("A_4_0", setting.A[4][0]);
  cfg.lookupValue("A_4_1", setting.A[4][1]);
  cfg.lookupValue("A_4_2", setting.A[4][2]);
  cfg.lookupValue("A_4_3", setting.A[4][3]);
  cfg.lookupValue("A_4_4", setting.A[4][4]);
  cfg.lookupValue("A_5_0", setting.A[5][0]);
  cfg.lookupValue("A_5_1", setting.A[5][1]);
  cfg.lookupValue("A_5_2", setting.A[5][2]);
  cfg.lookupValue("A_5_3", setting.A[5][3]);
  cfg.lookupValue("A_5_4", setting.A[5][4]);
  cfg.lookupValue("A_6_0", setting.A[6][0]);
  cfg.lookupValue("A_6_1", setting.A[6][1]);
  cfg.lookupValue("A_6_2", setting.A[6][2]);
  cfg.lookupValue("A_6_3", setting.A[6][3]);
  cfg.lookupValue("A_6_4", setting.A[6][4]);
  cfg.lookupValue("A_7_0", setting.A[7][0]);
  cfg.lookupValue("A_7_1", setting.A[7][1]);
  cfg.lookupValue("A_7_2", setting.A[7][2]);
  cfg.lookupValue("A_7_3", setting.A[7][3]);
  cfg.lookupValue("A_7_4", setting.A[7][4]);
  cfg.lookupValue("A_8_0", setting.A[8][0]);
  cfg.lookupValue("A_8_1", setting.A[8][1]);
  cfg.lookupValue("A_8_2", setting.A[8][2]);
  cfg.lookupValue("A_8_3", setting.A[8][3]);
  cfg.lookupValue("A_8_4", setting.A[8][4]);
  cfg.lookupValue("A_9_0", setting.A[9][0]);
  cfg.lookupValue("A_9_1", setting.A[9][1]);
  cfg.lookupValue("A_9_2", setting.A[9][2]);
  cfg.lookupValue("A_9_3", setting.A[9][3]);
  cfg.lookupValue("A_9_4", setting.A[9][4]);
  cfg.lookupValue("A_10_0", setting.A[10][0]);
  cfg.lookupValue("A_10_1", setting.A[10][1]);
  cfg.lookupValue("A_10_2", setting.A[10][2]);
  cfg.lookupValue("A_10_3", setting.A[10][3]);
  cfg.lookupValue("A_10_4", setting.A[10][4]);
  cfg.lookupValue("A_11_0", setting.A[11][0]);
  cfg.lookupValue("A_11_1", setting.A[11][1]);
  cfg.lookupValue("A_11_2", setting.A[11][2]);
  cfg.lookupValue("A_11_3", setting.A[11][3]);
  cfg.lookupValue("A_11_4", setting.A[11][4]);
  cfg.lookupValue("B_0_0", setting.B[0][0]);
  cfg.lookupValue("B_0_1", setting.B[0][1]);
  cfg.lookupValue("B_0_2", setting.B[0][2]);
  cfg.lookupValue("B_0_3", setting.B[0][3]);
  cfg.lookupValue("B_0_4", setting.B[0][4]);
  cfg.lookupValue("B_1_0", setting.B[1][0]);
  cfg.lookupValue("B_1_1", setting.B[1][1]);
  cfg.lookupValue("B_1_2", setting.B[1][2]);
  cfg.lookupValue("B_1_3", setting.B[1][3]);
  cfg.lookupValue("B_1_4", setting.B[1][4]);
  cfg.lookupValue("B_2_0", setting.B[2][0]);
  cfg.lookupValue("B_2_1", setting.B[2][1]);
  cfg.lookupValue("B_2_2", setting.B[2][2]);
  cfg.lookupValue("B_2_3", setting.B[2][3]);
  cfg.lookupValue("B_2_4", setting.B[2][4]);
  cfg.lookupValue("B_3_0", setting.B[3][0]);
  cfg.lookupValue("B_3_1", setting.B[3][1]);
  cfg.lookupValue("B_3_2", setting.B[3][2]);
  cfg.lookupValue("B_3_3", setting.B[3][3]);
  cfg.lookupValue("B_3_4", setting.B[3][4]);
  cfg.lookupValue("B_4_0", setting.B[4][0]);
  cfg.lookupValue("B_4_1", setting.B[4][1]);
  cfg.lookupValue("B_4_2", setting.B[4][2]);
  cfg.lookupValue("B_4_3", setting.B[4][3]);
  cfg.lookupValue("B_4_4", setting.B[4][4]);
  cfg.lookupValue("B_5_0", setting.B[5][0]);
  cfg.lookupValue("B_5_1", setting.B[5][1]);
  cfg.lookupValue("B_5_2", setting.B[5][2]);
  cfg.lookupValue("B_5_3", setting.B[5][3]);
  cfg.lookupValue("B_5_4", setting.B[5][4]);
  cfg.lookupValue("B_6_0", setting.B[6][0]);
  cfg.lookupValue("B_6_1", setting.B[6][1]);
  cfg.lookupValue("B_6_2", setting.B[6][2]);
  cfg.lookupValue("B_6_3", setting.B[6][3]);
  cfg.lookupValue("B_6_4", setting.B[6][4]);
  cfg.lookupValue("B_7_0", setting.B[7][0]);
  cfg.lookupValue("B_7_1", setting.B[7][1]);
  cfg.lookupValue("B_7_2", setting.B[7][2]);
  cfg.lookupValue("B_7_3", setting.B[7][3]);
  cfg.lookupValue("B_7_4", setting.B[7][4]);
  cfg.lookupValue("B_8_0", setting.B[8][0]);
  cfg.lookupValue("B_8_1", setting.B[8][1]);
  cfg.lookupValue("B_8_2", setting.B[8][2]);
  cfg.lookupValue("B_8_3", setting.B[8][3]);
  cfg.lookupValue("B_8_4", setting.B[8][4]);
  cfg.lookupValue("B_9_0", setting.B[9][0]);
  cfg.lookupValue("B_9_1", setting.B[9][1]);
  cfg.lookupValue("B_9_2", setting.B[9][2]);
  cfg.lookupValue("B_9_3", setting.B[9][3]);
  cfg.lookupValue("B_9_4", setting.B[9][4]);
  cfg.lookupValue("B_10_0", setting.B[10][0]);
  cfg.lookupValue("B_10_1", setting.B[10][1]);
  cfg.lookupValue("B_10_2", setting.B[10][2]);
  cfg.lookupValue("B_10_3", setting.B[10][3]);
  cfg.lookupValue("B_10_4", setting.B[10][4]);
  cfg.lookupValue("B_11_0", setting.B[11][0]);
  cfg.lookupValue("B_11_1", setting.B[11][1]);
  cfg.lookupValue("B_11_2", setting.B[11][2]);
  cfg.lookupValue("B_11_3", setting.B[11][3]);
  cfg.lookupValue("B_11_4", setting.B[11][4]);
  cfg.lookupValue("ALPHA_0_0", setting.ALPHA[0][0]);
  cfg.lookupValue("ALPHA_0_1", setting.ALPHA[0][1]);
  cfg.lookupValue("ALPHA_0_2", setting.ALPHA[0][2]);
  cfg.lookupValue("ALPHA_0_3", setting.ALPHA[0][3]);
  cfg.lookupValue("ALPHA_0_4", setting.ALPHA[0][4]);
  cfg.lookupValue("ALPHA_1_0", setting.ALPHA[1][0]);
  cfg.lookupValue("ALPHA_1_1", setting.ALPHA[1][1]);
  cfg.lookupValue("ALPHA_1_2", setting.ALPHA[1][2]);
  cfg.lookupValue("ALPHA_1_3", setting.ALPHA[1][3]);
  cfg.lookupValue("ALPHA_1_4", setting.ALPHA[1][4]); 
  cfg.lookupValue("ALPHA_2_0", setting.ALPHA[2][0]);
  cfg.lookupValue("ALPHA_2_1", setting.ALPHA[2][1]);
  cfg.lookupValue("ALPHA_2_2", setting.ALPHA[2][2]);
  cfg.lookupValue("ALPHA_2_3", setting.ALPHA[2][3]);
  cfg.lookupValue("ALPHA_2_4", setting.ALPHA[2][4]);  
  cfg.lookupValue("ALPHA_3_0", setting.ALPHA[3][0]);
  cfg.lookupValue("ALPHA_3_1", setting.ALPHA[3][1]);
  cfg.lookupValue("ALPHA_3_2", setting.ALPHA[3][2]);
  cfg.lookupValue("ALPHA_3_3", setting.ALPHA[3][3]);
  cfg.lookupValue("ALPHA_3_4", setting.ALPHA[3][4]);  
  cfg.lookupValue("ALPHA_4_0", setting.ALPHA[4][0]);
  cfg.lookupValue("ALPHA_4_1", setting.ALPHA[4][1]);
  cfg.lookupValue("ALPHA_4_2", setting.ALPHA[4][2]);
  cfg.lookupValue("ALPHA_4_3", setting.ALPHA[4][3]);
  cfg.lookupValue("ALPHA_4_4", setting.ALPHA[4][4]);  
  cfg.lookupValue("ALPHA_5_0", setting.ALPHA[5][0]);
  cfg.lookupValue("ALPHA_5_1", setting.ALPHA[5][1]);
  cfg.lookupValue("ALPHA_5_2", setting.ALPHA[5][2]);
  cfg.lookupValue("ALPHA_5_3", setting.ALPHA[5][3]);
  cfg.lookupValue("ALPHA_5_4", setting.ALPHA[5][4]);  
  cfg.lookupValue("ALPHA_6_0", setting.ALPHA[6][0]);
  cfg.lookupValue("ALPHA_6_1", setting.ALPHA[6][1]);
  cfg.lookupValue("ALPHA_6_2", setting.ALPHA[6][2]);
  cfg.lookupValue("ALPHA_6_3", setting.ALPHA[6][3]);
  cfg.lookupValue("ALPHA_6_4", setting.ALPHA[6][4]);  
  cfg.lookupValue("ALPHA_7_0", setting.ALPHA[7][0]);
  cfg.lookupValue("ALPHA_7_1", setting.ALPHA[7][1]);
  cfg.lookupValue("ALPHA_7_2", setting.ALPHA[7][2]);
  cfg.lookupValue("ALPHA_7_3", setting.ALPHA[7][3]);
  cfg.lookupValue("ALPHA_7_4", setting.ALPHA[7][4]);  
  cfg.lookupValue("ALPHA_8_0", setting.ALPHA[8][0]);
  cfg.lookupValue("ALPHA_8_1", setting.ALPHA[8][1]);
  cfg.lookupValue("ALPHA_8_2", setting.ALPHA[8][2]);
  cfg.lookupValue("ALPHA_8_3", setting.ALPHA[8][3]);
  cfg.lookupValue("ALPHA_8_4", setting.ALPHA[8][4]);  
  cfg.lookupValue("ALPHA_9_0", setting.ALPHA[9][0]);
  cfg.lookupValue("ALPHA_9_1", setting.ALPHA[9][1]);
  cfg.lookupValue("ALPHA_9_2", setting.ALPHA[9][2]);
  cfg.lookupValue("ALPHA_9_3", setting.ALPHA[9][3]);
  cfg.lookupValue("ALPHA_9_4", setting.ALPHA[9][4]);  
  cfg.lookupValue("ALPHA_10_0", setting.ALPHA[10][0]);
  cfg.lookupValue("ALPHA_10_1", setting.ALPHA[10][1]);
  cfg.lookupValue("ALPHA_10_2", setting.ALPHA[10][2]);
  cfg.lookupValue("ALPHA_10_3", setting.ALPHA[10][3]);
  cfg.lookupValue("ALPHA_10_4", setting.ALPHA[10][4]);  
  cfg.lookupValue("ALPHA_11_0", setting.ALPHA[11][0]);
  cfg.lookupValue("ALPHA_11_1", setting.ALPHA[11][1]);
  cfg.lookupValue("ALPHA_11_2", setting.ALPHA[11][2]);
  cfg.lookupValue("ALPHA_11_3", setting.ALPHA[11][3]);
  cfg.lookupValue("ALPHA_11_4", setting.ALPHA[11][4]);
  cfg.lookupValue("X_INITIAL_VALUE", setting.X_INITIAL_VALUE);
  cfg.lookupValue("Y_INITIAL_VALUE", setting.Y_INITIAL_VALUE);
  cfg.lookupValue("Z_INITIAL_VALUE", setting.Z_INITIAL_VALUE);
  //cfg.lookupValue("OMEGA_INITIAL_VALUE", setting.OMEGA_INITIAL_VALUE);
  cfg.lookupValue("OMEGA_T_SIZE", setting.OMEGA_T_SIZE);
  cfg.lookupValue("Z_SCALAR", setting.Z_SCALAR);
  cfg.lookupValue("LARGE_A", setting.LARGE_A);
  cfg.lookupValue("F_2", setting.F_2);
  cfg.lookupValue("ECG_STEP_NUM_PER_SEC", setting.ECG_STEP_NUM_PER_SEC);
 }
 catch(const libconfig::FileIOException &fioex){
  std::cout << "No found config file" << std::endl;exit(-1);
 }
 catch(const libconfig::ParseException &pex){
  std::cout << "Parse error" << std::endl;exit(-1);
 } 

 return setting;
}



int main(int argc, char* argv[]){
 if (argc < 3){
  std::cout << "./out configfile (ex: ./out setting.txt)\n";
  exit(-1);
 }

 Setting setting = read_config(argv[1], argv[2]);

 /* 子プロセスの数を設定 */
 const int child_num = CHILD_PROCESS_NUM;

int shared_memory_segment_id;
 struct shared_memory* shared_memory_pointer;

 shared_memory_pointer = new shared_memory;

 /* リアルタイムカウンタを初期化   */
 shared_memory_pointer->parent_count_last = 0;
 shared_memory_pointer->child_process_ecg_count = 0;
 shared_memory_pointer->parent_count = 1;


 /* リアルタイムカウンタの最大値を準備   */
 unsigned long long ull_limit_real_time_count = ULLONG_MAX; ull_limit_real_time_count -= 100; /* 適当な数字を引いておく */


 /* 共有メモリ識別子を取得 */
 if ((shared_memory_segment_id = shmget(IPC_PRIVATE, sizeof(struct shared_memory), IPC_CREAT | 0666)) == -1) {
  std::cerr << "Error : shmget : " << strerror(errno) << std::endl;
  exit(EXIT_FAILURE);
 }


/* 子プロセスの数だけ繰り返す */
 for (int child = 0; child < child_num; child++) {
 int child_cnt = child + 1;

 /* 子プロセス起動 */
 pid_t pid = fork();

 /* 子プロセスに共通の処理 */
 if (pid == 0) {

  output_message_standard(pid, child_cnt, "start.", "", "");

  /* 子プロセスに共有メモリをアタッチ */
  if ((shared_memory_pointer = (shared_memory*)shmat(shared_memory_segment_id, NULL, 0)) == (void*) - 1) {
   std::cerr << "Error : " << "child process No " << child_cnt << " shmat : " << strerror(errno) << std::endl;
   exit(EXIT_FAILURE);
  }

  output_message_standard(pid, child_cnt, "parent process PID:", getppid(), "");
  output_message_standard(pid, child_cnt, "child process  PID:", getpid(), "");

 /*                                              */
 /* 子プロセス1の処理                            */
 /*                                              */
 /* ECGの時系列を取得                            */
 if (child_cnt == 1) {

  /*                                    */
  /* 子プロセス1のECG生成に必要な初期化 */
  /*                                    */
  //int ecg_num_in = ECG_CHANNEL_NUM;
  //int mcsharry_variable_num_in = MCSHARRY_VARIABLE_NUM;
  //int variable_num_in = VARIABLE_NUM;
  //int distinct_point_num_in = DISTINCT_POINT_NUM;
  //double ecg_step_size_in = setting.ECG_STEP_SIZE;                 /* = 0.02  */
  //int ecg_one_cycle_step_num_in = setting.ECG_ONE_CYCLE_STEP_NUM;  /* = 5     */
  //std::vector<double> theta = setting.THETA;
  //std::vector<double> a_in = setting.A;
  //std::vector<double> b_in = setting.B;
  //std::vector<double> alpha_in = setting.ALPHA;
  //double x_initial_value_in = setting.X_INITIAL_VALUE;
  //double y_initial_value_in = setting.Y_INITIAL_VALUE;
  //double z_initial_value_in = setting.Z_INITIAL_VALUE;
  //double omega_initial_value = setting.OMEGA_INITIAL_VALUE;
  //int omega_t_size_in = setting.OMEGA_T_SIZE;
  //double z_scalar_in = setting.Z_SCALAR;
  //double large_a_in = setting.LARGE_A;
  //double f_2_in = setting.F_2;
  //int step_num_per_sec_in = setting.ECG_STEP_NUM_PER_SEC;

  /*                             */
  /* 暫定でハードコード ここから */
  /*                             */
  //double omega_initial_value = 2 * PI;
  //std::vector<std::vector<double>> theta_in = setting.THETA;
  //theta_in[0][0] = 0; theta_in[0][1] = PI / 12; theta_in[0][2] = PI / 2; theta_in[0][3] = -PI / 3; theta_in[0][4] = -PI / 12;
  //theta_in[1][0] = 0; theta_in[1][1] = PI / 12; theta_in[1][2] = PI / 2; theta_in[1][3] = -PI / 3; theta_in[1][4] = -PI / 12;
  //theta_in[2][0] = 0; theta_in[2][1] = PI / 12; theta_in[2][2] = PI / 2; theta_in[2][3] = -PI / 3; theta_in[2][4] = -PI / 12;
  //theta_in[3][0] = 0; theta_in[3][1] = PI / 12; theta_in[3][2] = PI / 2; theta_in[3][3] = -PI / 3; theta_in[3][4] = -PI / 12;
  //theta_in[4][0] = 0; theta_in[4][1] = PI / 12; theta_in[4][2] = PI / 2; theta_in[4][3] = -PI / 3; theta_in[4][4] = -PI / 12;
  //theta_in[5][0] = 0; theta_in[5][1] = PI / 12; theta_in[5][2] = PI / 2; theta_in[5][3] = -PI / 3; theta_in[5][4] = -PI / 12;
  //theta_in[6][0] = 0; theta_in[6][1] = PI / 12; theta_in[6][2] = PI / 2; theta_in[6][3] = -PI / 3; theta_in[6][4] = -PI / 12;
  //theta_in[7][0] = 0; theta_in[7][1] = PI / 12; theta_in[7][2] = PI / 2; theta_in[7][3] = -PI / 3; theta_in[7][4] = -PI / 12;
  //theta_in[8][0] = 0; theta_in[8][1] = PI / 12; theta_in[8][2] = PI / 2; theta_in[8][3] = -PI / 3; theta_in[8][4] = -PI / 12;
  //theta_in[9][0] = 0; theta_in[9][1] = PI / 12; theta_in[9][2] = PI / 2; theta_in[9][3] = -PI / 3; theta_in[9][4] = -PI / 12;
  //theta_in[10][0] = 0; theta_in[10][1] = PI / 12; theta_in[10][2] = PI / 2; theta_in[10][3] = -PI / 3; theta_in[10][4] = -PI / 12;
  //theta_in[11][0] = 0; theta_in[11][1] = PI / 12; theta_in[11][2] = PI / 2; theta_in[11][3] = -PI / 3; theta_in[11][4] = -PI / 12;
  /*                             */
  /* ここまで                    */
  /*                             */

  ///*                                         */
  ///* ECG生成モデルのパラメータ構造体への入力 */
  ///*                                         */
  //struct ecg_model_parameter* ecg_model_parameter_pointer;
  //ecg_model_parameter_pointer = new ecg_model_parameter;

  //ecg_model_parameter_pointer->ecg_num = ECG_CHANNEL_NUM;
  //ecg_model_parameter_pointer->mcsharry_variable_num = MCSHARRY_VARIABLE_NUM;
  //ecg_model_parameter_pointer->variable_num = VARIABLE_NUM;
  //ecg_model_parameter_pointer->distinct_point_num = DISTINCT_POINT_NUM;
  //ecg_model_parameter_pointer->step_size = setting.ECG_STEP_SIZE;               /* = 0.02  */
  //ecg_model_parameter_pointer->step_num_per_sec = setting.ECG_STEP_NUM_PER_SEC; /* = 500     */
  //ecg_model_parameter_pointer->step_num = setting.ECG_ONE_CYCLE_STEP_NUM;       /* = 5     */

  //ecg_model_parameter_pointer->theta = theta_in;
  //
  //ecg_model_parameter_pointer->a = setting.A;
  //ecg_model_parameter_pointer->b = setting.B;
  //ecg_model_parameter_pointer->alpha = setting.ALPHA;
  //ecg_model_parameter_pointer->x_initial = setting.X_INITIAL_VALUE;
  //ecg_model_parameter_pointer->y_initial = setting.Y_INITIAL_VALUE;
  //ecg_model_parameter_pointer->z_initial = setting.Z_INITIAL_VALUE;

  //ecg_model_parameter_pointer->omega_initial = 2 * PI;
  //
  //ecg_model_parameter_pointer->omega_t_size = setting.OMEGA_T_SIZE;
  //ecg_model_parameter_pointer->z_scalar = setting.Z_SCALAR;
  //ecg_model_parameter_pointer->large_a = setting.LARGE_A;
  //ecg_model_parameter_pointer->f_2 = setting.F_2;




   /*                             */
 /* 計算パラメータ作成 ここから */
 /*                             */
  int ecg_num_in = ECG_CHANNEL_NUM;   /* 12誘導 */
  int mcsharry_variable_num_in = MCSHARRY_VARIABLE_NUM;  /* x, y, z, omega の4つ */
  int distinct_point_num_in = DISTINCT_POINT_NUM;        /* P, Q, R, S, T の5つ */

  double step_size_in = 0.002;       /* [sec] */
  int step_num_per_sec_in = 500;     /* = 1 / 0.002  */
  //int step_num_in = 5000;            /* シミュレーションステップ数  */
  int step_num_in = 5;            /* シミュレーションステップ数  */

  std::vector<std::vector<double>> theta_in;  /* channel_num × distinct_point_num */
  std::vector<std::vector<double>> a_in;      /* channel_num × distinct_point_num */
  std::vector<std::vector<double>> b_in;      /* channel_num × distinct_point_num */
  std::vector<std::vector<double>> alpha_in;  /* channel_num × distinct_point_num */

  theta_in.resize(ecg_num_in, std::vector<double>(distinct_point_num_in, 0));
  a_in.resize(ecg_num_in, std::vector<double>(distinct_point_num_in, 0));
  b_in.resize(ecg_num_in, std::vector<double>(distinct_point_num_in, 0));
  alpha_in.resize(ecg_num_in, std::vector<double>(distinct_point_num_in, 0));

  /* theta */
 /* R,S,T,P,Q は インデックス 0,1,2,3,4 に対応 */
  theta_in[0][0] = 0; theta_in[0][1] = PI / 12; theta_in[0][2] = PI / 2; theta_in[0][3] = -PI / 3; theta_in[0][4] = -PI / 12;
  theta_in[1][0] = 0; theta_in[1][1] = PI / 12; theta_in[1][2] = PI / 2; theta_in[1][3] = -PI / 3; theta_in[1][4] = -PI / 12;
  theta_in[2][0] = 0; theta_in[2][1] = PI / 12; theta_in[2][2] = PI / 2; theta_in[2][3] = -PI / 3; theta_in[2][4] = -PI / 12;

  theta_in[3][0] = 0; theta_in[3][1] = PI / 12; theta_in[3][2] = PI / 2; theta_in[3][3] = -PI / 3; theta_in[3][4] = -PI / 12;
  theta_in[4][0] = 0; theta_in[4][1] = PI / 12; theta_in[4][2] = PI / 2; theta_in[4][3] = -PI / 3; theta_in[4][4] = -PI / 12;
  theta_in[5][0] = 0; theta_in[5][1] = PI / 12; theta_in[5][2] = PI / 2; theta_in[5][3] = -PI / 3; theta_in[5][4] = -PI / 12;

  theta_in[6][0] = 0; theta_in[6][1] = PI / 12; theta_in[6][2] = PI / 2; theta_in[6][3] = -PI / 3; theta_in[6][4] = -PI / 12;
  theta_in[7][0] = 0; theta_in[7][1] = PI / 12; theta_in[7][2] = PI / 2; theta_in[7][3] = -PI / 3; theta_in[7][4] = -PI / 12;
  theta_in[8][0] = 0; theta_in[8][1] = PI / 12; theta_in[8][2] = PI / 2; theta_in[8][3] = -PI / 3; theta_in[8][4] = -PI / 12;

  theta_in[9][0] = 0; theta_in[9][1] = PI / 12; theta_in[9][2] = PI / 2; theta_in[9][3] = -PI / 3; theta_in[9][4] = -PI / 12;
  theta_in[10][0] = 0; theta_in[10][1] = PI / 12; theta_in[10][2] = PI / 2; theta_in[10][3] = -PI / 3; theta_in[10][4] = -PI / 12;
  theta_in[11][0] = 0; theta_in[11][1] = PI / 12; theta_in[11][2] = PI / 2; theta_in[11][3] = -PI / 3; theta_in[11][4] = -PI / 12;

  /* a */
  /* r,s,t,p,q は インデックス 0,1,2,3,4 に対応 */
  a_in[0][0] = 30.0; a_in[0][1] = -7.5; a_in[0][2] = 0.75; a_in[0][3] = 1.2; a_in[0][4] = -5.0;
  a_in[1][0] = 30.0; a_in[1][1] = -7.5; a_in[1][2] = 0.75; a_in[1][3] = 1.2; a_in[1][4] = -5.0;
  a_in[2][0] = 30.0; a_in[2][1] = -7.5; a_in[2][2] = 0.75; a_in[2][3] = 1.2; a_in[2][4] = -5.0;

  a_in[3][0] = 30.0; a_in[3][1] = -7.5; a_in[3][2] = 0.75; a_in[3][3] = 1.2; a_in[3][4] = -5.0;
  a_in[4][0] = 30.0; a_in[4][1] = -7.5; a_in[4][2] = 0.75; a_in[4][3] = 1.2; a_in[4][4] = -5.0;
  a_in[5][0] = 30.0; a_in[5][1] = -7.5; a_in[5][2] = 0.75; a_in[5][3] = 1.2; a_in[5][4] = -5.0;

  a_in[6][0] = 30.0; a_in[6][1] = -7.5; a_in[6][2] = 0.75; a_in[6][3] = 1.2; a_in[6][4] = -5.0;
  a_in[7][0] = 30.0; a_in[7][1] = -7.5; a_in[7][2] = 0.75; a_in[7][3] = 1.2; a_in[7][4] = -5.0;
  a_in[8][0] = 30.0; a_in[8][1] = -7.5; a_in[8][2] = 0.75; a_in[8][3] = 1.2; a_in[8][4] = -5.0;

  a_in[9][0] = 30.0; a_in[9][1] = -7.5; a_in[9][2] = 0.75; a_in[9][3] = 1.2; a_in[9][4] = -5.0;
  a_in[10][0] = 30.0; a_in[10][1] = -7.5; a_in[10][2] = 0.75; a_in[10][3] = 1.2; a_in[10][4] = -5.0;
  a_in[11][0] = 30.0; a_in[11][1] = -7.5; a_in[11][2] = 0.75; a_in[11][3] = 1.2; a_in[11][4] = -5.0;

  /* b */
  /* r,s,t,p,q は インデックス 0,1,2,3,4 に対応 */
  b_in[0][0] = 0.1; b_in[0][1] = 0.1; b_in[0][2] = 0.4; b_in[0][3] = 0.25; b_in[0][4] = 0.1;
  b_in[1][0] = 0.1; b_in[1][1] = 0.1; b_in[1][2] = 0.4; b_in[1][3] = 0.25; b_in[1][4] = 0.1;
  b_in[2][0] = 0.1; b_in[2][1] = 0.1; b_in[2][2] = 0.4; b_in[2][3] = 0.25; b_in[2][4] = 0.1;

  b_in[3][0] = 0.1; b_in[3][1] = 0.1; b_in[3][2] = 0.4; b_in[3][3] = 0.25; b_in[3][4] = 0.1;
  b_in[4][0] = 0.1; b_in[4][1] = 0.1; b_in[4][2] = 0.4; b_in[4][3] = 0.25; b_in[4][4] = 0.1;
  b_in[5][0] = 0.1; b_in[5][1] = 0.1; b_in[5][2] = 0.4; b_in[5][3] = 0.25; b_in[5][4] = 0.1;

  b_in[6][0] = 0.1; b_in[6][1] = 0.1; b_in[6][2] = 0.4; b_in[6][3] = 0.25; b_in[6][4] = 0.1;
  b_in[7][0] = 0.1; b_in[7][1] = 0.1; b_in[7][2] = 0.4; b_in[7][3] = 0.25; b_in[7][4] = 0.1;
  b_in[8][0] = 0.1; b_in[8][1] = 0.1; b_in[8][2] = 0.4; b_in[8][3] = 0.25; b_in[8][4] = 0.1;

  b_in[9][0] = 0.1; b_in[9][1] = 0.1; b_in[9][2] = 0.4; b_in[9][3] = 0.25; b_in[9][4] = 0.1;
  b_in[10][0] = 0.1; b_in[10][1] = 0.1; b_in[10][2] = 0.4; b_in[10][3] = 0.25; b_in[10][4] = 0.1;
  b_in[11][0] = 0.1; b_in[11][1] = 0.1; b_in[11][2] = 0.4; b_in[11][3] = 0.25; b_in[11][4] = 0.1;


  /* 生体医工学会用のパラメータ */
  /* alpha */
  /* R,S,T,P,Q は インデックス 0,1,2,3,4 に対応 */
  alpha_in[0][3] = 0.5616; alpha_in[0][4] = -0.03012; alpha_in[0][0] = 0.9; alpha_in[0][1] = 0.4969; alpha_in[0][2] = 0.3964;
  alpha_in[1][3] = 0.65; alpha_in[1][4] = -0.0602; alpha_in[1][0] = 1.2; alpha_in[1][1] = 0.9941; alpha_in[1][2] = 0.4;
  alpha_in[2][3] = 0.3; alpha_in[2][4] = -0.03005; alpha_in[2][0] = 0.3; alpha_in[2][1] = 0.497; alpha_in[2][2] = -0.2;

  alpha_in[3][3] = -0.6483; alpha_in[3][4] = 0.0367; alpha_in[3][0] = -1.1; alpha_in[3][1] = -0.5964; alpha_in[3][2] = -0.6;
  alpha_in[4][3] = 0.1; alpha_in[4][4] = -0.0133; alpha_in[4][0] = 0.4; alpha_in[4][1] = 0.5; alpha_in[4][2] = 0.2;
  alpha_in[5][3] = 0.64455; alpha_in[5][4] = -0.0478; alpha_in[5][0] = 0.7949; alpha_in[5][1] = 0.7949; alpha_in[5][2] = 0.7008;

  alpha_in[6][3] = -0.4; alpha_in[6][4] = -1.6; alpha_in[6][0] = -0.7; alpha_in[6][1] = 0.3; alpha_in[6][2] = 0.4;
  alpha_in[7][3] = 0.3; alpha_in[7][4] = -0.02; alpha_in[7][0] = 0.3; alpha_in[7][1] = 4.0; alpha_in[7][2] = 0.7;
  alpha_in[8][3] = 0.6; alpha_in[8][4] = -0.02969; alpha_in[8][0] = 0.7; alpha_in[8][1] = 3.854; alpha_in[8][2] = 0.6;

  alpha_in[9][3] = 0.5; alpha_in[9][4] = 1.4796; alpha_in[9][0] = 0.9; alpha_in[9][1] = 2.2; alpha_in[9][2] = 0.6;
  alpha_in[10][3] = 0.4; alpha_in[10][4] = 0.981; alpha_in[10][0] = 1.4; alpha_in[10][1] = 1.3; alpha_in[10][2] = 0.5;
  alpha_in[11][3] = 0.4; alpha_in[11][4] = -0.0367; alpha_in[11][0] = 1.3; alpha_in[11][1] = 0.5; alpha_in[11][2] = 0.3;


  ///* alpha */
  ///* R,S,T,P,Q は インデックス 0,1,2,3,4 に対応 */
  //alpha_in[0][3] = 0.5616; alpha_in[0][4] = -0.03012; alpha_in[0][0] = 0.4969; alpha_in[0][1] = 0.4969; alpha_in[0][2] = 0.3964;
  //alpha_in[1][3] = 1.6768; alpha_in[1][4] = -0.0602; alpha_in[1][0] = 0.9941; alpha_in[1][1] = 0.9941; alpha_in[1][2] = 1.199;
  //alpha_in[2][3] = 1.1243; alpha_in[2][4] = -0.03005; alpha_in[2][0] = 0.497; alpha_in[2][1] = 0.497; alpha_in[2][2] = 0.801;

  //alpha_in[3][3] = -0.6483; alpha_in[3][4] = 0.0367; alpha_in[3][0] = -0.5964; alpha_in[3][1] = -0.5964; alpha_in[3][2] = -0.7009;
  //alpha_in[4][3] = 0.8046; alpha_in[4][4] = -0.0133; alpha_in[4][0] = 0.1985; alpha_in[4][1] = 0.1985; alpha_in[4][2] = -0.397;
  //alpha_in[5][3] = 0.64455; alpha_in[5][4] = -0.0478; alpha_in[5][0] = 0.7949; alpha_in[5][1] = 0.7949; alpha_in[5][2] = 0.7008;

  //alpha_in[6][3] = -2.4565; alpha_in[6][4] = -2.979; alpha_in[6][0] = -0.9841; alpha_in[6][1] = 0.12983; alpha_in[6][2] = 0.983;
  //alpha_in[7][3] = 1.485; alpha_in[7][4] = -0.088; alpha_in[7][0] = 0.6; alpha_in[7][1] = 5.508; alpha_in[7][2] = 2.3995;
  //alpha_in[8][3] = 1.3145; alpha_in[8][4] = -0.02969; alpha_in[8][0] = 1.23854; alpha_in[8][1] = 3.854; alpha_in[8][2] = 2.303;

  //alpha_in[9][3] = 0.0709; alpha_in[9][4] = 1.4796; alpha_in[9][0] = 1.3662; alpha_in[9][1] = 3.9862; alpha_in[9][2] = 1.6701;
  //alpha_in[10][3] = 0.093; alpha_in[10][4] = 0.981; alpha_in[10][0] = 1.363; alpha_in[10][1] = 3.2863; alpha_in[10][2] = 1.5036;
  //alpha_in[11][3] = 0.7485; alpha_in[11][4] = -0.0367; alpha_in[11][0] = 1.2972; alpha_in[11][1] = 2.8972; alpha_in[11][2] = 1.9003;

  double x_initial_in = 1;
  double y_initial_in = 0;
  double z_initial_in = 0.04;
  double omega_initial_in = 2 * PI;
  int omega_t_size_in = 500 * 10000;
  double z_scalar_in = 25;
  double large_a_in = 0;
  double f_2_in = 0;

  /*          */
  /* ここまで */
  /*          */



  /*                                                  */
  /* ECG生成モデルのパラメータ構造体への入力ここから  */
  /*                                                  */
  struct ecg_model_parameter* ecg_model_parameter_pointer;
  ecg_model_parameter_pointer = new ecg_model_parameter;

  ecg_model_parameter_pointer->ecg_num = ecg_num_in;
  ecg_model_parameter_pointer->mcsharry_variable_num = mcsharry_variable_num_in;
  ecg_model_parameter_pointer->variable_num = mcsharry_variable_num_in - 1;
  ecg_model_parameter_pointer->distinct_point_num = distinct_point_num_in;

  ecg_model_parameter_pointer->step_size = step_size_in;
  ecg_model_parameter_pointer->step_num_per_sec = step_num_per_sec_in;
  ecg_model_parameter_pointer->step_num = step_num_in;

  ecg_model_parameter_pointer->theta = theta_in;
  ecg_model_parameter_pointer->a = a_in;
  ecg_model_parameter_pointer->b = b_in;
  ecg_model_parameter_pointer->alpha = alpha_in;

  ecg_model_parameter_pointer->x_initial = x_initial_in;
  ecg_model_parameter_pointer->y_initial = y_initial_in;
  ecg_model_parameter_pointer->z_initial = z_initial_in;
  ecg_model_parameter_pointer->omega_initial = omega_initial_in;
  ecg_model_parameter_pointer->omega_t_size = omega_t_size_in;

  ecg_model_parameter_pointer->z_scalar = z_scalar_in;

  ecg_model_parameter_pointer->large_a = large_a_in;
  ecg_model_parameter_pointer->f_2 = f_2_in;

  /*          */
  /* ここまで */
  /*          */






  /* インスタンス生成 */
  class fasade_child_process_ecg fasade_child_process_ecg_obj(ecg_model_parameter_pointer);

  /*                                            */
  /* 子プロセス1 リアルタイムECG生成 ループ開始 */
  /*                                            */
  while (true) {

   /* カウンタ更新の検出 */
   while (shared_memory_pointer->child_process_ecg_count == shared_memory_pointer->parent_count_last) {}

   /* 自プロセスのカウンタを更新 */
   unsigned long long tmp_count_child = shared_memory_pointer->child_process_ecg_count;
   shared_memory_pointer->child_process_ecg_count = (++tmp_count_child) % ull_limit_real_time_count;

   /* ECG生成と共有メモリへの書き込み */
   fasade_child_process_ecg_obj.fasade_child_process_ecg_execute(shared_memory_pointer);
  }
 }

 /* 子プロセスから共有メモリを切り離す */
 if (shmdt(shared_memory_pointer) == -1) {
  std::cerr << "Error : " << "child process No " << child_cnt << " : shmdt : " << strerror(errno) << std::endl;
  exit(EXIT_FAILURE);
 }
 output_message_standard(pid, child_cnt, "end.","", "");

 /* 子プロセスを終了 */
 exit(0);
 }

/*            */
/* 親プロセス */
/*            */
 else if (pid > 0) {

  output_message_standard(pid, child_cnt, "parent process PID:", getpid(), "");
  output_message_standard(pid, child_cnt, "wait for child process", child_cnt, "to finish.");

  /* 親プロセスに共有メモリをアタッチ */
  if ((shared_memory_pointer = (shared_memory*)shmat(shared_memory_segment_id, NULL, 0)) == (void*)-1) {
   std::cerr << "Error : " << "parent process No " << child_cnt << " shmat : " << strerror(errno) << std::endl;
   exit(EXIT_FAILURE);
  }

  /* 親プロセスのMQTT送信に必要な初期化 */
  const char* ip = setting.IP.c_str();                 /* = "150.82.92.210"    */
  int port = setting.PORT;                             /* = 1883               */
  int keepalive = setting.KEEPALIVE;                   /* = 60                 */
  const char* username = setting.USERNAME.c_str();     /* = "uchiumi"          */
  const char* password = setting.PASSWORD.c_str();     /* = "3214neknek"       */
  const char* id = setting.ID.c_str();                 /* = "generator_server" */
  bool clean_session = setting.CLEAN_SESSION;          /* = true               */
  const char* topic = setting.TOPIC.c_str();           /* = "pseudo_ecg_topic" */
  int quality_of_service = setting.QUALITY_OF_SERVICE; /* = 0                  */
  bool retain = setting.RETAIN;                        /* = false              */
  int step_size_msec = setting.STEP_SIZE_MSEC;         /* = 2                  */

  class fasade_parent_process_mqtt fasade_parent_process_mqtt_obj(ip, port, keepalive, username, password, id, clean_session, topic, quality_of_service, retain, step_size_msec);

  /*                                            */
  /* 親プロセス リアルタイムMQTT送信 ループ開始 */
  /* ほぼ 0.01[sec] 毎にカウンタを更新          */
  /*                                            */
  while (true) {

   /* 親プロセスのカウンタを更新してMQTT送信 */
   unsigned long long tmp_count_parent_last = shared_memory_pointer->parent_count_last;
   unsigned long long tmp_count_parent = shared_memory_pointer->parent_count;
   shared_memory_pointer->parent_count_last = (++tmp_count_parent_last) % ull_limit_real_time_count; /* ← これの実行直後に子プロセスの待ちが解除され子プロセスの処理が開始する  */
   shared_memory_pointer->parent_count = (++tmp_count_parent) % ull_limit_real_time_count;

   std::cout << "parent count in shared memory :" << shared_memory_pointer->parent_count << std::endl;

   /* 0.01[sec] 毎 */
   usleep(10000); /* = 1000000 * 0.01  */

   /*          */
   /* MQTT送信 */
   /*          */
   fasade_parent_process_mqtt_obj.fasade_parent_process_mqtt_execute(shared_memory_pointer);
  }

  wait(NULL);

  /* 親プロセスから共有メモリを切り離す */
  if (shmdt(shared_memory_pointer) == -1) {
   std::cerr << "Error : " << "parent process No " << child_cnt << " : shmdt : " << strerror(errno) << std::endl;
   exit(EXIT_FAILURE);
  }
 }
}

 /* 共有メモリを解放 */
 if (shmctl(shared_memory_segment_id, IPC_RMID, NULL) == -1) {
  std::cerr << "Error : shmctl : " << strerror(errno) << std::endl;
  exit(-1);
 }
 std::cout << "[parent process] parent process finished." << std::endl;
 /*  output_message_standard(pid, child_cnt,,,) でなくてよい  */

 return 0;
}
