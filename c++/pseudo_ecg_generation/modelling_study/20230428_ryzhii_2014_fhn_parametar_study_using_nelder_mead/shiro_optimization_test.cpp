
#include <iostream>
#include <fstream>
#include <iomanip>
//#include <cmath>
#include <vector>
#include "nelmin.h"
#include "generate_ecg.h"
#include "ryzhii_parameter.h"

template<class T> inline bool chmax(T& a, T b) { if (a < b) { a = b; return true; } return false; }
template<class T> inline bool chmin(T& a, T b) { if (a > b) { a = b; return true; } return false; }

/* ecgの目標値 */
double ref_max;
double ref_min;

double ecg_function(std::vector<double>& x) { 

 /* パラメータを初期値に初期化 */
 ryzhii_parameter ryzhii_parameter_obj;

 /* ecg生成を x の関数として考えるための処理 */
 //ryzhii_parameter_obj.k[2] = x[0];
 //ryzhii_parameter_obj.c[2] = x[1];
 //ryzhii_parameter_obj.h[2] = x[2];
 //ryzhii_parameter_obj.g[2] = x[3];
 
 ryzhii_parameter_obj.k[2] = x[0];
 ryzhii_parameter_obj.c[2] = x[1];
 ryzhii_parameter_obj.w_1[2] = x[2];
 ryzhii_parameter_obj.w_2[2] = x[3];
 ryzhii_parameter_obj.b[2] = x[4];
 ryzhii_parameter_obj.h[2] = x[5];
 ryzhii_parameter_obj.g[2] = x[6];

 //ryzhii_parameter_obj.k[2] = x[0];
 //ryzhii_parameter_obj.c[2] = x[1];
 //ryzhii_parameter_obj.w_1[2] = x[2];
 //ryzhii_parameter_obj.w_2[2] = x[3];
 //ryzhii_parameter_obj.b[2] = x[4];
 //ryzhii_parameter_obj.h[2] = x[5];
 //ryzhii_parameter_obj.g[2] = x[6];
 //ryzhii_parameter_obj.large_c[2] = x[7];

 /* 心電図の生成 計算実行 ここから */
 ecg_signals ecg_obj(ryzhii_parameter_obj.step_size,
  ryzhii_parameter_obj.step_num,
  ryzhii_parameter_obj.a,
  ryzhii_parameter_obj.f,
  ryzhii_parameter_obj.e,
  ryzhii_parameter_obj.u,
  ryzhii_parameter_obj.d,
  ryzhii_parameter_obj.large_k,
  ryzhii_parameter_obj.k,
  ryzhii_parameter_obj.c,
  ryzhii_parameter_obj.w_1,
  ryzhii_parameter_obj.w_2,
  ryzhii_parameter_obj.b,
  ryzhii_parameter_obj.h,
  ryzhii_parameter_obj.g,
  ryzhii_parameter_obj.large_c,
  ryzhii_parameter_obj.x_initial_value,
  ryzhii_parameter_obj.y_initial_value,
  ryzhii_parameter_obj.z_initial_value,
  ryzhii_parameter_obj.v_initial_value,
  ryzhii_parameter_obj.delay_step_y,
  ryzhii_parameter_obj.alpha);

 ecg_obj.generate_12_lead_ecg_signals_one_cycle(ryzhii_parameter_obj.ecg_result,
  ryzhii_parameter_obj.x_result,
  ryzhii_parameter_obj.y_result,
  ryzhii_parameter_obj.z_result,
  ryzhii_parameter_obj.v_result);
 /* ここまで */


 //std::cout << "----------------------------------" << std::endl;
 //std::cout << "k_2 = " << x[0] << std::endl;
 //std::cout << "c_2 = " << x[1] << std::endl;
 //std::cout << "w_1_2 = " << x[2] << std::endl;
 //std::cout << "w_2_2 = " << x[3] << std::endl;
 ////std::cout << "b_2 = " << x[4] << std::endl;
 //std::cout << "h_2 = " << x[4] << std::endl;
 //std::cout << "g_2 = " << x[5] << std::endl;
 ////std::cout << "large_c_3 = " << x[7] << std::endl;

 /* fx は 比 あるいは 幅 になるよね  */
 /* 4～5[sec] 間の最大値と最小値の比を取得 */
 double fx = 0;
 double ecg_max = 0;
 double ecg_min = 1000000000000000000;
 for (int i = 2000; i < 2500; i++) {
  chmax(ecg_max, ryzhii_parameter_obj.ecg_result[i][0]);
  chmin(ecg_min, ryzhii_parameter_obj.ecg_result[i][0]);
 }

 //std::cout << "ecg_max = " << ecg_max << std::endl;
 //std::cout << "ecg_min = " << ecg_min << std::endl;


 /* 目的関数 */
 double lambda_max = 1;
 double lambda_min = 1;
 
 fx += lambda_max * (ref_max - ecg_max) * (ref_max - ecg_max);
 fx += lambda_min * (ref_min - ecg_min) * (ref_min - ecg_min);

 //std::cout << "fx = " << fx << std::endl;

 return fx;
}

void optimize_ecg(std::vector<double>& x) {
 int n = 7, i;
 //int n = 8, i;
 Data<double> in(n);

 /* 目的関数の変数の初期値 */
 //in.start[0] = 1e4;   // k[2] =1e4;
 //in.start[1] = 0.12;  // c[2] = 0.12;
 //in.start[2] = 0.008; // h[2] = 0.008;
 //in.start[3] = 0.09;  // g[2] = 0.09;

 in.start[0] = 1e4;    // k[2] =1e4;
 in.start[1] = 0.12; // c[2] = 0.12;
 in.start[2] = 0.12; // w_1[2] = 0.12;
 in.start[3] = 1.1;  // w_2[2] = 1.1;
 in.start[4] = 0.015;    // b[2] = 0.015;
 in.start[5] = 0.008;   // h[2] = 0.008;
 in.start[6] = 0.09; // g[2] = 0.09;

 //in.start[0] = 1e4;    // k[2] =1e4;
 //in.start[1] = 0.12; // c[2] = 0.12;
 //in.start[2] = 0.12; // w_1[2] = 0.12;
 //in.start[3] = 1.1;  // w_2[2] = 1.1;
 //in.start[4] = 0.015;    // b[2] = 0.015;
 //in.start[5] = 0.008;   // h[2] = 0.008;
 //in.start[6] = 0.09; // g[2] = 0.09;
 //in.start[7] = 9e-5;   // large_c[3] = 9e-5;

 /* フィッティング最適化(Nelder–Mead)を実行 */
 nelmin(ecg_function, &in);

 std::cout << " Return code IFAULT = " << in.ifault << "\n";
 std::cout << "\n";
 std::cout << " Estimate of minimizing value X*:\n";
 std::cout << "\n";
 for (i = 0; i < n; i++) {
  std::cout << " " << std::setw(14) << in.xmin[i] << "\n";
 }
 std::cout << "\n";
 std::cout << " F(X*) = " << in.ynewlo << "\n";
 std::cout << "\n";
 std::cout << " Number of iterations = " << in.icount << "\n";
 std::cout << " Number of restarts =  " << in.numres << "\n";


 /* 戻り値に格納 */
 x.resize(n);
 for (int i = 0; i < n; i++) {
  x[i] = in.xmin[i];
 }

}

void generate_ecg(std::vector<double> x) {

 /* パラメータを初期値に初期化 */
 ryzhii_parameter ryzhii_parameter_obj;

 //ryzhii_parameter_obj.k[2] = x[0];
 //ryzhii_parameter_obj.c[2] = x[1];
 //ryzhii_parameter_obj.h[2] = x[2];
 //ryzhii_parameter_obj.g[2] = x[3];

 ryzhii_parameter_obj.k[2] = x[0];
 ryzhii_parameter_obj.c[2] = x[1];
 ryzhii_parameter_obj.w_1[2] = x[2];
 ryzhii_parameter_obj.w_2[2] = x[3];
 ryzhii_parameter_obj.b[2] = x[4];
 ryzhii_parameter_obj.h[2] = x[5];
 ryzhii_parameter_obj.g[2] = x[6];

 //ryzhii_parameter_obj.k[2] = x[0];
 //ryzhii_parameter_obj.c[2] = x[1];
 //ryzhii_parameter_obj.w_1[2] = x[2];
 //ryzhii_parameter_obj.w_2[2] = x[3];
 //ryzhii_parameter_obj.b[2] = x[4];
 //ryzhii_parameter_obj.h[2] = x[5];
 //ryzhii_parameter_obj.g[2] = x[6];
 //ryzhii_parameter_obj.large_c[2] = x[7];


 /* 心電図の生成 計算実行 ここから */
 ecg_signals ecg_obj(ryzhii_parameter_obj.step_size,
  ryzhii_parameter_obj.step_num,
  ryzhii_parameter_obj.a,
  ryzhii_parameter_obj.f,
  ryzhii_parameter_obj.e,
  ryzhii_parameter_obj.u,
  ryzhii_parameter_obj.d,
  ryzhii_parameter_obj.large_k,
  ryzhii_parameter_obj.k,
  ryzhii_parameter_obj.c,
  ryzhii_parameter_obj.w_1,
  ryzhii_parameter_obj.w_2,
  ryzhii_parameter_obj.b,
  ryzhii_parameter_obj.h,
  ryzhii_parameter_obj.g,
  ryzhii_parameter_obj.large_c,
  ryzhii_parameter_obj.x_initial_value,
  ryzhii_parameter_obj.y_initial_value,
  ryzhii_parameter_obj.z_initial_value,
  ryzhii_parameter_obj.v_initial_value,
  ryzhii_parameter_obj.delay_step_y,
  ryzhii_parameter_obj.alpha);

 ecg_obj.generate_12_lead_ecg_signals_one_cycle(ryzhii_parameter_obj.ecg_result,
  ryzhii_parameter_obj.x_result,
  ryzhii_parameter_obj.y_result,
  ryzhii_parameter_obj.z_result,
  ryzhii_parameter_obj.v_result);
 /* ここまで */



 /* テキストファイルに書き込みして出力 ここから */
 std::string filename = "ecg_result.txt";
 std::ofstream outputfile(filename);
 for (int i = 0; i < ryzhii_parameter_obj.step_num; i++) {
  for (int j = 0; j < ryzhii_parameter_obj.ecg_num; j++) {
   outputfile << ryzhii_parameter_obj.ecg_result[i][j] << "\t";
  }
  outputfile << "\n";
 }
 outputfile.close();
 


 int x_num = ryzhii_parameter_obj.vdp_variable_num;
 int z_num = ryzhii_parameter_obj.fhn_variable_num;
 int x_y_z_v_num = x_num + x_num + z_num + z_num;
 std::vector<std::vector<double>> x_y_z_v_result(ryzhii_parameter_obj.step_num, std::vector<double>(x_y_z_v_num));
 for (int i = 0; i < ryzhii_parameter_obj.step_num; i++) {
  for (int j = 0; j < x_y_z_v_num; j++) {
   if (j < x_num) {
	x_y_z_v_result[i][j] = ryzhii_parameter_obj.x_result[i][j];
   }
   else if (x_num <= j && j < x_num + x_num) {
	x_y_z_v_result[i][j] = ryzhii_parameter_obj.y_result[i][j - x_num];
   }
   else if (x_num + x_num <= j && j < x_num + x_num + z_num) {
	x_y_z_v_result[i][j] = ryzhii_parameter_obj.z_result[i][j - (x_num + x_num)];
   }
   else {
	x_y_z_v_result[i][j] = ryzhii_parameter_obj.v_result[i][j - (x_num + x_num + z_num)];
   }
  }
 }

 std::string filename1 = "x_y_z_v_result.txt";
 std::ofstream outputfile1(filename1);
 for (int i = 0; i < ryzhii_parameter_obj.step_num; i++) {
  for (int j = 0; j < x_y_z_v_num; j++) {
   outputfile1 << x_y_z_v_result[i][j] << "\t";
  }
  outputfile1 << "\n";
 }
 outputfile1.close();
 /* ここまで */

}

void select_ref_case(int pattern, double scalar) {

 if (pattern == 1) {
  ref_max = 1;
  ref_min = 0;
 }
 else if (pattern == 2) {
  ref_max = 0.9;
  ref_min = -0.1;
 }
 else if (pattern == 3) {
  ref_max = 0.8;
  ref_min = -0.2;
 }
 else if (pattern == 4) {
  ref_max = 0.7;
  ref_min = -0.3;
 }
 else if (pattern == 5) {
  ref_max = 0.6;
  ref_min = -0.4;
 }
 else if (pattern == 6) {
  ref_max = 0.5;
  ref_min = -0.5;
 }

 ref_max *= scalar;
 ref_min *= scalar;
}



int main()
{

 int pattern = 5;
 double scalar = 1;
 ref_max = 0.7;
 ref_min = -0.5;

 select_ref_case(pattern, scalar);

 /* フィッティングして最適な x を取得 */
 std::vector<double> x;
 optimize_ecg(x);

 /* 最適な x でECGを生成 */
 generate_ecg(x);
 
 return 0;
}