

#include "pecg.h"

#include <libconfig.h++>




Setting read_config(const char* in_data_write, const char* in_model) {

 Setting setting;
 libconfig::Config cfg;

 try {

  cfg.readFile(in_data_write);
  setting.CONFIG_FILENAME_DATA_WRITE = std::string(in_data_write);

  cfg.lookupValue("JSON_STORE_DIR", setting.JSON_STORE_DIR);
  cfg.lookupValue("JSON_FILE_NAME", setting.JSON_FILE_NAME);
  cfg.lookupValue("JSON_SAMPLING_MSEC", setting.JSON_SAMPLING_MSEC);
  cfg.lookupValue("JSON_KEY_TIME", setting.JSON_KEY_TIME);
  cfg.lookupValue("JSON_KEY_CHANNEL", setting.JSON_KEY_CHANNEL);
  cfg.lookupValue("JSON_KEY_CHANNEL_00", setting.JSON_KEY_CHANNEL_00);
  cfg.lookupValue("JSON_KEY_CHANNEL_01", setting.JSON_KEY_CHANNEL_01);
  cfg.lookupValue("JSON_KEY_CHANNEL_02", setting.JSON_KEY_CHANNEL_02);
  cfg.lookupValue("JSON_KEY_CHANNEL_03", setting.JSON_KEY_CHANNEL_03);
  cfg.lookupValue("JSON_KEY_CHANNEL_04", setting.JSON_KEY_CHANNEL_04);
  cfg.lookupValue("JSON_KEY_CHANNEL_05", setting.JSON_KEY_CHANNEL_05);
  cfg.lookupValue("JSON_KEY_CHANNEL_06", setting.JSON_KEY_CHANNEL_06);
  cfg.lookupValue("JSON_KEY_CHANNEL_07", setting.JSON_KEY_CHANNEL_07);
  cfg.lookupValue("JSON_KEY_CHANNEL_08", setting.JSON_KEY_CHANNEL_08);
  cfg.lookupValue("JSON_KEY_CHANNEL_09", setting.JSON_KEY_CHANNEL_09);
  cfg.lookupValue("JSON_KEY_CHANNEL_10", setting.JSON_KEY_CHANNEL_10);
  cfg.lookupValue("JSON_KEY_CHANNEL_11", setting.JSON_KEY_CHANNEL_11);
 

  cfg.readFile(in_model);
  setting.CONFIG_FILENAME_MODEL = std::string(in_model);

  cfg.lookupValue("STEP_SIZE", setting.STEP_SIZE);
  cfg.lookupValue("ONE_SEGMENT_BPM", setting.ONE_SEGMENT_BPM);
  cfg.lookupValue("ECG_CHANNEL_NUM", setting.ECG_CHANNEL_NUM);
  cfg.lookupValue("FITTING_FUNCTION_NUM", setting.FITTING_FUNCTION_NUM);
  cfg.lookupValue("FUNCTION_PARAMETER_NUM", setting.FUNCTION_PARAMETER_NUM);

  cfg.lookupValue("NORM_DIST_NOISE_SEED", setting.NORM_DIST_NOISE_SEED);
  cfg.lookupValue("NORM_DIST_NOISE_MEAN", setting.NORM_DIST_NOISE_MEAN);
  cfg.lookupValue("NORM_DIST_NOISE_STDEV", setting.NORM_DIST_NOISE_STDEV);

  setting.A.resize(setting.ECG_CHANNEL_NUM,
   std::vector<double>(setting.FITTING_FUNCTION_NUM));
  setting.MU.resize(setting.ECG_CHANNEL_NUM,
   std::vector<double>(setting.FITTING_FUNCTION_NUM));
  setting.SIGMA.resize(setting.ECG_CHANNEL_NUM,
   std::vector<double>(setting.FITTING_FUNCTION_NUM));
  setting.SCALAR.resize(setting.ECG_CHANNEL_NUM,
   std::vector<double>(setting.FITTING_FUNCTION_NUM));

  /* lead Ⅰ */
  cfg.lookupValue("A_00_0", setting.A[0][0]);
  cfg.lookupValue("A_00_1", setting.A[0][1]);
  cfg.lookupValue("A_00_2", setting.A[0][2]);
  cfg.lookupValue("A_00_3", setting.A[0][3]);
  cfg.lookupValue("A_00_4", setting.A[0][4]); 
  cfg.lookupValue("A_00_5", setting.A[0][5]);
  cfg.lookupValue("A_00_6", setting.A[0][6]);
  cfg.lookupValue("MU_00_0", setting.MU[0][0]);
  cfg.lookupValue("MU_00_1", setting.MU[0][1]);
  cfg.lookupValue("MU_00_2", setting.MU[0][2]);
  cfg.lookupValue("MU_00_3", setting.MU[0][3]);
  cfg.lookupValue("MU_00_4", setting.MU[0][4]);
  cfg.lookupValue("MU_00_5", setting.MU[0][5]);
  cfg.lookupValue("MU_00_6", setting.MU[0][6]);
  cfg.lookupValue("SIGMA_00_0", setting.SIGMA[0][0]);
  cfg.lookupValue("SIGMA_00_1", setting.SIGMA[0][1]);
  cfg.lookupValue("SIGMA_00_2", setting.SIGMA[0][2]);
  cfg.lookupValue("SIGMA_00_3", setting.SIGMA[0][3]);
  cfg.lookupValue("SIGMA_00_4", setting.SIGMA[0][4]);
  cfg.lookupValue("SIGMA_00_5", setting.SIGMA[0][5]);
  cfg.lookupValue("SIGMA_00_6", setting.SIGMA[0][6]);
  cfg.lookupValue("SCALAR_00_0", setting.SCALAR[0][0]);
  cfg.lookupValue("SCALAR_00_1", setting.SCALAR[0][1]);
  cfg.lookupValue("SCALAR_00_2", setting.SCALAR[0][2]);
  cfg.lookupValue("SCALAR_00_3", setting.SCALAR[0][3]);
  cfg.lookupValue("SCALAR_00_4", setting.SCALAR[0][4]);
  cfg.lookupValue("SCALAR_00_5", setting.SCALAR[0][5]);
  cfg.lookupValue("SCALAR_00_6", setting.SCALAR[0][6]);


  /* lead Ⅱ */
  cfg.lookupValue("A_01_0", setting.A[1][0]);
  cfg.lookupValue("A_01_1", setting.A[1][1]);
  cfg.lookupValue("A_01_2", setting.A[1][2]);
  cfg.lookupValue("A_01_3", setting.A[1][3]);
  cfg.lookupValue("A_01_4", setting.A[1][4]);
  cfg.lookupValue("A_01_5", setting.A[1][5]);
  cfg.lookupValue("A_01_6", setting.A[1][6]);
  cfg.lookupValue("MU_01_0", setting.MU[1][0]);
  cfg.lookupValue("MU_01_1", setting.MU[1][1]);
  cfg.lookupValue("MU_01_2", setting.MU[1][2]);
  cfg.lookupValue("MU_01_3", setting.MU[1][3]);
  cfg.lookupValue("MU_01_4", setting.MU[1][4]);
  cfg.lookupValue("MU_01_5", setting.MU[1][5]);
  cfg.lookupValue("MU_01_6", setting.MU[1][6]);
  cfg.lookupValue("SIGMA_01_0", setting.SIGMA[1][0]);
  cfg.lookupValue("SIGMA_01_1", setting.SIGMA[1][1]);
  cfg.lookupValue("SIGMA_01_2", setting.SIGMA[1][2]);
  cfg.lookupValue("SIGMA_01_3", setting.SIGMA[1][3]);
  cfg.lookupValue("SIGMA_01_4", setting.SIGMA[1][4]);
  cfg.lookupValue("SIGMA_01_5", setting.SIGMA[1][5]);
  cfg.lookupValue("SIGMA_01_6", setting.SIGMA[1][6]);
  cfg.lookupValue("SCALAR_01_0", setting.SCALAR[1][0]);
  cfg.lookupValue("SCALAR_01_1", setting.SCALAR[1][1]);
  cfg.lookupValue("SCALAR_01_2", setting.SCALAR[1][2]);
  cfg.lookupValue("SCALAR_01_3", setting.SCALAR[1][3]);
  cfg.lookupValue("SCALAR_01_4", setting.SCALAR[1][4]);
  cfg.lookupValue("SCALAR_01_5", setting.SCALAR[1][5]);
  cfg.lookupValue("SCALAR_01_6", setting.SCALAR[1][6]);

  /* lead V1 */
  cfg.lookupValue("A_06_0", setting.A[6][0]);
  cfg.lookupValue("A_06_1", setting.A[6][1]);
  cfg.lookupValue("A_06_2", setting.A[6][2]);
  cfg.lookupValue("A_06_3", setting.A[6][3]);
  cfg.lookupValue("A_06_4", setting.A[6][4]);
  cfg.lookupValue("A_06_5", setting.A[6][5]);
  cfg.lookupValue("A_06_6", setting.A[6][6]);
  cfg.lookupValue("MU_06_0", setting.MU[6][0]);
  cfg.lookupValue("MU_06_1", setting.MU[6][1]);
  cfg.lookupValue("MU_06_2", setting.MU[6][2]);
  cfg.lookupValue("MU_06_3", setting.MU[6][3]);
  cfg.lookupValue("MU_06_4", setting.MU[6][4]);
  cfg.lookupValue("MU_06_5", setting.MU[6][5]);
  cfg.lookupValue("MU_06_6", setting.MU[6][6]);
  cfg.lookupValue("SIGMA_06_0", setting.SIGMA[6][0]);
  cfg.lookupValue("SIGMA_06_1", setting.SIGMA[6][1]);
  cfg.lookupValue("SIGMA_06_2", setting.SIGMA[6][2]);
  cfg.lookupValue("SIGMA_06_3", setting.SIGMA[6][3]);
  cfg.lookupValue("SIGMA_06_4", setting.SIGMA[6][4]);
  cfg.lookupValue("SIGMA_06_5", setting.SIGMA[6][5]);
  cfg.lookupValue("SIGMA_06_6", setting.SIGMA[6][6]);
  cfg.lookupValue("SCALAR_06_0", setting.SCALAR[6][0]);
  cfg.lookupValue("SCALAR_06_1", setting.SCALAR[6][1]);
  cfg.lookupValue("SCALAR_06_2", setting.SCALAR[6][2]);
  cfg.lookupValue("SCALAR_06_3", setting.SCALAR[6][3]);
  cfg.lookupValue("SCALAR_06_4", setting.SCALAR[6][4]);
  cfg.lookupValue("SCALAR_06_5", setting.SCALAR[6][5]);
  cfg.lookupValue("SCALAR_06_6", setting.SCALAR[6][6]);

  /* lead V2 */
  cfg.lookupValue("A_07_0", setting.A[7][0]);
  cfg.lookupValue("A_07_1", setting.A[7][1]);
  cfg.lookupValue("A_07_2", setting.A[7][2]);
  cfg.lookupValue("A_07_3", setting.A[7][3]);
  cfg.lookupValue("A_07_4", setting.A[7][4]);
  cfg.lookupValue("A_07_5", setting.A[7][5]);
  cfg.lookupValue("A_07_6", setting.A[7][6]);
  cfg.lookupValue("MU_07_0", setting.MU[7][0]);
  cfg.lookupValue("MU_07_1", setting.MU[7][1]);
  cfg.lookupValue("MU_07_2", setting.MU[7][2]);
  cfg.lookupValue("MU_07_3", setting.MU[7][3]);
  cfg.lookupValue("MU_07_4", setting.MU[7][4]);
  cfg.lookupValue("MU_07_5", setting.MU[7][5]);
  cfg.lookupValue("MU_07_6", setting.MU[7][6]);
  cfg.lookupValue("SIGMA_07_0", setting.SIGMA[7][0]);
  cfg.lookupValue("SIGMA_07_1", setting.SIGMA[7][1]);
  cfg.lookupValue("SIGMA_07_2", setting.SIGMA[7][2]);
  cfg.lookupValue("SIGMA_07_3", setting.SIGMA[7][3]);
  cfg.lookupValue("SIGMA_07_4", setting.SIGMA[7][4]);
  cfg.lookupValue("SIGMA_07_5", setting.SIGMA[7][5]);
  cfg.lookupValue("SIGMA_07_6", setting.SIGMA[7][6]);
  cfg.lookupValue("SCALAR_07_0", setting.SCALAR[7][0]);
  cfg.lookupValue("SCALAR_07_1", setting.SCALAR[7][1]);
  cfg.lookupValue("SCALAR_07_2", setting.SCALAR[7][2]);
  cfg.lookupValue("SCALAR_07_3", setting.SCALAR[7][3]);
  cfg.lookupValue("SCALAR_07_4", setting.SCALAR[7][4]);
  cfg.lookupValue("SCALAR_07_5", setting.SCALAR[7][5]);
  cfg.lookupValue("SCALAR_07_6", setting.SCALAR[7][6]);

  /* lead V3 */
  cfg.lookupValue("A_08_0", setting.A[8][0]);
  cfg.lookupValue("A_08_1", setting.A[8][1]);
  cfg.lookupValue("A_08_2", setting.A[8][2]);
  cfg.lookupValue("A_08_3", setting.A[8][3]);
  cfg.lookupValue("A_08_4", setting.A[8][4]);
  cfg.lookupValue("A_08_5", setting.A[8][5]);
  cfg.lookupValue("A_08_6", setting.A[8][6]);
  cfg.lookupValue("MU_08_0", setting.MU[8][0]);
  cfg.lookupValue("MU_08_1", setting.MU[8][1]);
  cfg.lookupValue("MU_08_2", setting.MU[8][2]);
  cfg.lookupValue("MU_08_3", setting.MU[8][3]);
  cfg.lookupValue("MU_08_4", setting.MU[8][4]);
  cfg.lookupValue("MU_08_5", setting.MU[8][5]);
  cfg.lookupValue("MU_08_6", setting.MU[8][6]);
  cfg.lookupValue("SIGMA_08_0", setting.SIGMA[8][0]);
  cfg.lookupValue("SIGMA_08_1", setting.SIGMA[8][1]);
  cfg.lookupValue("SIGMA_08_2", setting.SIGMA[8][2]);
  cfg.lookupValue("SIGMA_08_3", setting.SIGMA[8][3]);
  cfg.lookupValue("SIGMA_08_4", setting.SIGMA[8][4]);
  cfg.lookupValue("SIGMA_08_5", setting.SIGMA[8][5]);
  cfg.lookupValue("SIGMA_08_6", setting.SIGMA[8][6]);
  cfg.lookupValue("SCALAR_08_0", setting.SCALAR[8][0]);
  cfg.lookupValue("SCALAR_08_1", setting.SCALAR[8][1]);
  cfg.lookupValue("SCALAR_08_2", setting.SCALAR[8][2]);
  cfg.lookupValue("SCALAR_08_3", setting.SCALAR[8][3]);
  cfg.lookupValue("SCALAR_08_4", setting.SCALAR[8][4]);
  cfg.lookupValue("SCALAR_08_5", setting.SCALAR[8][5]);
  cfg.lookupValue("SCALAR_08_6", setting.SCALAR[8][6]);

  /* lead V4 */
  cfg.lookupValue("A_09_0", setting.A[9][0]);
  cfg.lookupValue("A_09_1", setting.A[9][1]);
  cfg.lookupValue("A_09_2", setting.A[9][2]);
  cfg.lookupValue("A_09_3", setting.A[9][3]);
  cfg.lookupValue("A_09_4", setting.A[9][4]);
  cfg.lookupValue("A_09_5", setting.A[9][5]);
  cfg.lookupValue("A_09_6", setting.A[9][6]);
  cfg.lookupValue("MU_09_0", setting.MU[9][0]);
  cfg.lookupValue("MU_09_1", setting.MU[9][1]);
  cfg.lookupValue("MU_09_2", setting.MU[9][2]);
  cfg.lookupValue("MU_09_3", setting.MU[9][3]);
  cfg.lookupValue("MU_09_4", setting.MU[9][4]);
  cfg.lookupValue("MU_09_5", setting.MU[9][5]);
  cfg.lookupValue("MU_09_6", setting.MU[9][6]);
  cfg.lookupValue("SIGMA_09_0", setting.SIGMA[9][0]);
  cfg.lookupValue("SIGMA_09_1", setting.SIGMA[9][1]);
  cfg.lookupValue("SIGMA_09_2", setting.SIGMA[9][2]);
  cfg.lookupValue("SIGMA_09_3", setting.SIGMA[9][3]);
  cfg.lookupValue("SIGMA_09_4", setting.SIGMA[9][4]);
  cfg.lookupValue("SIGMA_09_5", setting.SIGMA[9][5]);
  cfg.lookupValue("SIGMA_09_6", setting.SIGMA[9][6]);
  cfg.lookupValue("SCALAR_09_0", setting.SCALAR[9][0]);
  cfg.lookupValue("SCALAR_09_1", setting.SCALAR[9][1]);
  cfg.lookupValue("SCALAR_09_2", setting.SCALAR[9][2]);
  cfg.lookupValue("SCALAR_09_3", setting.SCALAR[9][3]);
  cfg.lookupValue("SCALAR_09_4", setting.SCALAR[9][4]);
  cfg.lookupValue("SCALAR_09_5", setting.SCALAR[9][5]);
  cfg.lookupValue("SCALAR_09_6", setting.SCALAR[9][6]);

  /* lead V5 */
  cfg.lookupValue("A_10_0", setting.A[10][0]);
  cfg.lookupValue("A_10_1", setting.A[10][1]);
  cfg.lookupValue("A_10_2", setting.A[10][2]);
  cfg.lookupValue("A_10_3", setting.A[10][3]);
  cfg.lookupValue("A_10_4", setting.A[10][4]);
  cfg.lookupValue("A_10_5", setting.A[10][5]);
  cfg.lookupValue("A_10_6", setting.A[10][6]);
  cfg.lookupValue("MU_10_0", setting.MU[10][0]);
  cfg.lookupValue("MU_10_1", setting.MU[10][1]);
  cfg.lookupValue("MU_10_2", setting.MU[10][2]);
  cfg.lookupValue("MU_10_3", setting.MU[10][3]);
  cfg.lookupValue("MU_10_4", setting.MU[10][4]);
  cfg.lookupValue("MU_10_5", setting.MU[10][5]);
  cfg.lookupValue("MU_10_6", setting.MU[10][6]);
  cfg.lookupValue("SIGMA_10_0", setting.SIGMA[10][0]);
  cfg.lookupValue("SIGMA_10_1", setting.SIGMA[10][1]);
  cfg.lookupValue("SIGMA_10_2", setting.SIGMA[10][2]);
  cfg.lookupValue("SIGMA_10_3", setting.SIGMA[10][3]);
  cfg.lookupValue("SIGMA_10_4", setting.SIGMA[10][4]);
  cfg.lookupValue("SIGMA_10_5", setting.SIGMA[10][5]);
  cfg.lookupValue("SIGMA_10_6", setting.SIGMA[10][6]);
  cfg.lookupValue("SCALAR_10_0", setting.SCALAR[10][0]);
  cfg.lookupValue("SCALAR_10_1", setting.SCALAR[10][1]);
  cfg.lookupValue("SCALAR_10_2", setting.SCALAR[10][2]);
  cfg.lookupValue("SCALAR_10_3", setting.SCALAR[10][3]);
  cfg.lookupValue("SCALAR_10_4", setting.SCALAR[10][4]);
  cfg.lookupValue("SCALAR_10_5", setting.SCALAR[10][5]);
  cfg.lookupValue("SCALAR_10_6", setting.SCALAR[10][6]);

  /* lead V6 */
  cfg.lookupValue("A_11_0", setting.A[11][0]);
  cfg.lookupValue("A_11_1", setting.A[11][1]);
  cfg.lookupValue("A_11_2", setting.A[11][2]);
  cfg.lookupValue("A_11_3", setting.A[11][3]);
  cfg.lookupValue("A_11_4", setting.A[11][4]);
  cfg.lookupValue("A_11_5", setting.A[11][5]);
  cfg.lookupValue("A_11_6", setting.A[11][6]);
  cfg.lookupValue("MU_11_0", setting.MU[11][0]);
  cfg.lookupValue("MU_11_1", setting.MU[11][1]);
  cfg.lookupValue("MU_11_2", setting.MU[11][2]);
  cfg.lookupValue("MU_11_3", setting.MU[11][3]);
  cfg.lookupValue("MU_11_4", setting.MU[11][4]);
  cfg.lookupValue("MU_11_5", setting.MU[11][5]);
  cfg.lookupValue("MU_11_6", setting.MU[11][6]);
  cfg.lookupValue("SIGMA_11_0", setting.SIGMA[11][0]);
  cfg.lookupValue("SIGMA_11_1", setting.SIGMA[11][1]);
  cfg.lookupValue("SIGMA_11_2", setting.SIGMA[11][2]);
  cfg.lookupValue("SIGMA_11_3", setting.SIGMA[11][3]);
  cfg.lookupValue("SIGMA_11_4", setting.SIGMA[11][4]);
  cfg.lookupValue("SIGMA_11_5", setting.SIGMA[11][5]);
  cfg.lookupValue("SIGMA_11_6", setting.SIGMA[11][6]);
  cfg.lookupValue("SCALAR_11_0", setting.SCALAR[11][0]);
  cfg.lookupValue("SCALAR_11_1", setting.SCALAR[11][1]);
  cfg.lookupValue("SCALAR_11_2", setting.SCALAR[11][2]);
  cfg.lookupValue("SCALAR_11_3", setting.SCALAR[11][3]);
  cfg.lookupValue("SCALAR_11_4", setting.SCALAR[11][4]);
  cfg.lookupValue("SCALAR_11_5", setting.SCALAR[11][5]);
  cfg.lookupValue("SCALAR_11_6", setting.SCALAR[11][6]);

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

 /* データ書き込みの設定ファイルと数理モデルの設定ファイル */  
 if (argc < 4) {
  std::cout << "./out configfile (ex: ./out setting.txt)\n";
  exit(-1);
 }

 /* 設定ファイル読み込み */
 Setting setting = read_config(argv[1], argv[2]);


 /* セグメント数 */
 int segmemt_num = std::atoi(argv[3]);

 /* セグメント長の配列を取得 */
 std::vector<double> segment_length;
 double bpm = setting.ONE_SEGMENT_BPM;
 double seg_len = (double)60 / bpm;
 segment_length = constant_rr_interval(segmemt_num, seg_len);


 /* ステップサイズ */
 double step_size = setting.STEP_SIZE;

 /* セグメントのステップ数の配列を取得 */
 std::vector<int> segment_step_num(segmemt_num);
 for (int i = 0; i < segmemt_num; i++) {
  segment_step_num[i] = segment_length[i] / step_size;
 }

 /* 現在のUNIX時間ミリ秒を取得する */
 long long initial_msec = get_actual_msec_since_epoch();

 /* JSON作成用           */
 /* サンプリングのミリ秒 */
 long long sampling_msec = setting.JSON_SAMPLING_MSEC;

 /* .json ファイルパス*/
 std::string json_file_path
  = setting.JSON_STORE_DIR + "/" + setting.JSON_FILE_NAME;


 /* 12誘導の 12 */
 int channel_num = setting.ECG_CHANNEL_NUM;

 /* ノイズのための乱数オブジェクト */
 normal_dist_random_number rn_obj(
  setting.NORM_DIST_NOISE_SEED,
  setting.NORM_DIST_NOISE_MEAN,
  setting.NORM_DIST_NOISE_STDEV);

 /*                                     */
 /* セグメントの数だけ繰り返す ここから */
 /*                                     */
 for (int i = 0; i < segmemt_num; i++) {

  int step_num = segment_step_num[i];
  std::vector<std::vector<double>> ecg_time_series(channel_num, std::vector<double>(step_num));

  /* 疑似ECG生成 12回繰り返し */
  for (int channel = 0; channel < channel_num; channel++) {
   
   /* Ⅲ,aVR,aVL,aVF は合成なのでスキップ */
   if (2 <= channel && channel <= 5) {
	continue;
   }
   
   /* 1セグメントの波形を作成 */
   ecg_time_series[channel] = gaussian_ecg_one_segment(setting, channel, step_num);
  
  }

  /* Ⅰ,Ⅱ から Ⅲ,aVR,aVL,aVF を作成 */
  synthesize(ecg_time_series, channel_num);

  /* 各誘導に同じ値のノイズを付加 */
  add_noise_to_ecg(ecg_time_series, rn_obj);
    
  /* セグメントのステップ数だけ繰り返す */
  for (int index = 0; index < step_num; index++) {

   /* JSONを作成して追記モードで書き込む */
   create_ecg_json_and_append(setting, index, ecg_time_series, initial_msec, json_file_path);
   initial_msec += sampling_msec;

  }

 }
 /*                                     */
 /* セグメントの数だけ繰り返す ここまで */
 /*                                     */



 return 0;
}