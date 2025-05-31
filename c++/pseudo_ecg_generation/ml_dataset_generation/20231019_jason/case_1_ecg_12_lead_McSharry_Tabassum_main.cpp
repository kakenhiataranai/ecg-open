// ecg_12_lead_McSharry_Tabassum.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

//#include "ecg_McSharry_model_using_fftw.h"


/* case_1 */
#include "case_1_ecg_McSharry_model_using_fftw.h"

#include "ecg_model_parameter.h"


#define PI 3.1415926535897932384626433832795028841971

#define ECG_CHANNEL_NUM 12
#define MCSHARRY_VARIABLE_NUM 4
#define DISTINCT_POINT_NUM 5
// #define FREQUENCY_MODAL_NUM 2




int main()
{

 /*                             */
 /* 計算パラメータ作成 ここから */
 /*                             */
 int ecg_num_in = ECG_CHANNEL_NUM;   /* 12誘導 */
 int mcsharry_variable_num_in = MCSHARRY_VARIABLE_NUM;  /* x, y, z, omega の4つ */
 int distinct_point_num_in = DISTINCT_POINT_NUM;        /* P, Q, R, S, T の5つ */

 double step_size_in = 0.002;       /* [sec] */
 int step_num_per_sec_in = 500;     /* = 1 / 0.002  */
 int step_num_in = 50000;            /* シミュレーションステップ数  */


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
 /* R,S,T,P,Q は インデックス 0,1,2,3,4 に対応 */
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
 /* R,S,T,P,Q は インデックス 0,1,2,3,4 に対応 */
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
 //double omega_initial_in = 2 * PI;
 double omega_initial_in = 2 * PI * 70 / 60;  /* 70 bpm */
 int omega_t_size_in = 10 * 10000;
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





 /*                                     */
 /* 計算結果出力用のパラメータここから  */
 /*                                     */
 int result_column_num = 5;

 /* 結果を格納する配列 サイズは 12 × (step_num × 4) */
 std::vector<std::vector<std::vector<double>>> result(ecg_num_in, std::vector<std::vector<double>>(step_num_in, std::vector<double>(result_column_num)));
 /*          */
 /* ここまで */
 /*          */






 /* 心電図の生成 計算実行 ここから */
 //ecg_signals ecg_obj(step_size_in, step_num_in, step_num_per_sec_in); 
 ecg_signals ecg_obj(ecg_model_parameter_pointer);
 ecg_obj.generate_ecg_signal_one_cycle(result);
 /* ここまで */



 /* Ⅲ, aVR, aVL, aVF を合成から作成 */
 int lead_1_channel = 0;
 int lead_2_channel = 1;
 int z_index = 3;
 for (int channel = 0; channel < ecg_num_in; channel++) {
  if (2 <= channel && channel <= 5) {
   for (int i = 0; i < step_num_in; i++) {
	switch (channel) {
	case 2:   /* Ⅲ */
	 result[channel][i][z_index] = result[lead_2_channel][i][z_index] - result[lead_1_channel][i][z_index];
	 break;
	case 3:   /* aVR */
	 result[channel][i][z_index] = (-1) * (result[lead_1_channel][i][z_index] + result[lead_2_channel][i][z_index]) * 0.5;
	  break;
	case 4:   /* aVL */
	 result[channel][i][z_index] = result[lead_1_channel][i][z_index] - result[lead_2_channel][i][z_index] * 0.5;
	 break;
	case 5:   /* aVF */
	 result[channel][i][z_index] = result[lead_2_channel][i][z_index] - result[lead_1_channel][i][z_index] * 0.5;
	 break;
	}   
   }
  }
 }




 /* テキストファイルに書き込みして出力 ここから */ 
 for (int channel = 0; channel < ecg_num_in; channel++) {

  std::string channel_str = std::to_string(channel);
  if (channel < 10) {
   channel_str = "0" + channel_str;
  }
  std::string filename = "result" + (channel_str) +  ".txt";  /* t, x, y, z (ecg), omega */
  std::ofstream outputfile(filename);

  for (int i = 0; i < step_num_in; i++) {
   for (int j = 0; j < result_column_num; j++) {
	outputfile << result[channel][i][j] << "\t";
   }
   outputfile << "\n";
  }
  outputfile.close();
 }

 std::vector<std::vector<double>> result_ecg(step_num_in, std::vector<double>(ecg_num_in));
 //for (int i = 0; i < step_num; i++) {
 // result_ecg[i][0] = result[0][i][0];  /* 時間のインデックスは 0 */
 //} 
 for (int channel = 0; channel < ecg_num_in; channel++) {
  for (int i = 0; i < step_num_in; i++) {
   result_ecg[i][channel] = result[channel][i][3];  /* z のインデックスは 3 */
  }
 }
 std::string filename = "result_ecg.txt";  /* t, z (ecg) 12列 */
 std::ofstream outputfile(filename);
 for (int i = 0; i < step_num_in; i++) {
  for (int j = 0; j < ecg_num_in; j++) {
   outputfile << result_ecg[i][j] << "\t";
  }
  outputfile << "\n";
 }
 outputfile.close();
 /* ここまで */


  /* .csvファイルに書き込みして出力 ここから */
 // Open a file stream for writing
 std::ofstream file("result_ecg_case_1_.csv");

 // Write the data to the file
 for (int i = 0; i < step_num_in; i++) {
  for (int j = 0; j < ecg_num_in; j++) {
   // Write the element to the file
   file << result_ecg[i][j];
   // Add a comma after the element, unless it's the last element in the row
   if (j < ecg_num_in - 1) {
	file << ",";
   }
  }
  // Add a new line after each row
  file << "\n";
 }
 // Close the file stream
 file.close();
 /*  ここまで */

 return 0;
}

