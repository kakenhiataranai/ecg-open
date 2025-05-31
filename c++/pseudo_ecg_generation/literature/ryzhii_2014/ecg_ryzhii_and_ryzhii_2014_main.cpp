#include <iostream>
#include <fstream>

#include "generate_ecg.h"

int main(){


 /* 計算パラメータ作成 ここから */
 double step_size = 0.002;   /* [sec] */
 int step_num = 5000;        /* シミュレーションステップ数  */

 int vdp_variable_num = 3;
 std::vector<double> a(vdp_variable_num); a[0] = 40; a[1] = 50; a[2] = 50;
 std::vector<double> f(vdp_variable_num); f[0] = 22; f[1] = 8.4; f[2] = 1.5;
 std::vector<double> e(vdp_variable_num); e[0] = 3.5; e[1] = 5; e[2] = 12;
 
 //std::vector<double> u(vdp_variable_num); u[0] = 0.69; u[1] = 0.69; u[2] = 0.69;
  std::vector<double> u(vdp_variable_num); u[0] = 0.6889; u[1] = 0.6889; u[2] = 0.6889;

 std::vector<double> d(vdp_variable_num); d[0] = 3; d[1] = 3; d[2] = 3;
 std::vector<double> large_k(vdp_variable_num); large_k[0] = 0; large_k[1] = 22; large_k[2] = 22;
 
 int fhn_variable_num = 4;
 
 //std::vector<double> k(fhn_variable_num); k[0] = 2e3; k[1] = 1e2; k[2] = 1e4; k[3] = 2e3;
 std::vector<double> k(fhn_variable_num); k[0] = 2e3; k[1] = 4e2; k[2] = 1e4; k[3] = 2e3;

 //std::vector<double> c(fhn_variable_num); c[0] = 0.26; c[1] = 0.12; c[2] = 0.12; c[3] = 0.1;
 std::vector<double> c(fhn_variable_num); c[0] = 0.26; c[1] = 0.26; c[2] = 0.12; c[3] = 0.1;
 
 //std::vector<double> w_1(fhn_variable_num); w_1[0] = 0.13; w_1[1] = 0.12; w_1[2] = 0.12; w_1[3] = 0.22;
 std::vector<double> w_1(fhn_variable_num); w_1[0] = 0.13; w_1[1] = 0.19; w_1[2] = 0.12; w_1[3] = 0.22;

 std::vector<double> w_2(fhn_variable_num); w_2[0] = 1.0; w_2[1] = 1.1; w_2[2] = 1.1; w_2[3] = 0.8;
 std::vector<double> b(fhn_variable_num); b[0] = 0; b[1] = 0; b[2] = 0.015;  b[3] = 0;
 
 //std::vector<double> h(fhn_variable_num); h[0] = 0.004; h[1] = 0.008; h[2] = 0.008; h[3] = 0.008;
 std::vector<double> h(fhn_variable_num); h[0] = 0.004; h[1] = 0.004; h[2] = 0.008; h[3] = 0.008;

 //std::vector<double> g(fhn_variable_num); g[0] = 0.4; g[1] = 0.09; g[2] = 0.09; g[3] = 0.1;
 /* Ryzhii and Ryzhii(2014)では d  */
 std::vector<double> g(fhn_variable_num); g[0] = 0.4; g[1] = 0.4; g[2] = 0.09; g[3] = 0.1;


 std::vector<double> large_c(fhn_variable_num); large_c[0] = 4e-5; large_c[1] = 4e-5; large_c[2] = 9e-5; large_c[3] = 6e-5;

 double x_initial_value = -0.1;
 double y_initial_value = 0.025;
 double z_initial_value = 0;
 double v_initial_value = 0;
 
 int delay_step_y = 46;      /* 時間遅れステップ数 論文では 0.092[s]  0.002 * 46 = 0.092 */
 

 int ecg_num = 12;    /* 12誘導心電図 */
 

 std::vector<std::vector<double>> alpha(ecg_num, std::vector<double>(fhn_variable_num));

 /* Ryzhii and Ryzhii (2014) はⅡ誘導しか使わないので全部同じにしておく */
 alpha[0][0] = 1.0;   alpha[0][1] = 1.0; alpha[0][2] = 1.0;  alpha[0][3] = 1.0;
 alpha[1][0] = 1.0;   alpha[1][1] = 1.0;  alpha[1][2] = 1.0;  alpha[1][3] = 1.0;
 alpha[2][0] = 1.0;   alpha[2][1] = 1.0; alpha[2][2] = 1.0;   alpha[2][3] = 1.0;

 alpha[3][0] = 1.0;  alpha[3][1] = 1.0;   alpha[3][2] = 1.0; alpha[3][3] = 1.0;
 alpha[4][0] = 1.0;   alpha[4][1] = 1.0;  alpha[4][2] = 1.0;  alpha[4][3] = 1.0;
 alpha[5][0] = 1.0;  alpha[5][1] = 1.0;  alpha[5][2] = 1.0;  alpha[5][3] = 1.0;

 alpha[6][0] = 1.0;   alpha[6][1] = 1.0;   alpha[6][2] = 1.0; alpha[6][3] = 1.0;
 alpha[7][0] = 1.0;   alpha[7][1] = 1.0;    alpha[7][2] = 1.0; alpha[7][3] = 1.0;
 alpha[8][0] = 1.0;   alpha[8][1] = 1.0;   alpha[8][2] = 1.0;  alpha[8][3] = 1.0;

 alpha[9][0] = 1.0;    alpha[9][1] = 1.0;   alpha[9][2] = 1.0;  alpha[9][3] = 1.0;
 alpha[10][0] = 1.0;  alpha[10][1] = 1.0;  alpha[10][2] = 1.0; alpha[10][3] = 1.0;
 alpha[11][0] = 1.0;  alpha[11][1] = 1.0; alpha[11][2] = 1.0;  alpha[11][3] = 1.0;
 /* ここまで */




 /* 出力用 */
 std::vector<std::vector<double>> x_result(step_num, std::vector<double>(vdp_variable_num));
 std::vector<std::vector<double>> y_result(step_num, std::vector<double>(vdp_variable_num));
 std::vector<std::vector<double>> z_result(step_num, std::vector<double>(fhn_variable_num));
 std::vector<std::vector<double>> v_result(step_num, std::vector<double>(fhn_variable_num));
 std::vector<std::vector<double>> ecg_result(step_num, std::vector<double>(ecg_num));



 /* 心電図の生成 計算実行 ここから */
 ecg_signals ecg_obj(step_size, 
                     step_num, 
                     a,
                     f,
                     e,
                     u,
                     d,
                     large_k,
                     k,
                     c,
                     w_1,
                     w_2,
                     b,
                     h,
                     g,
                     large_c,
                     x_initial_value, 
                     y_initial_value, 
                     z_initial_value, 
                     v_initial_value, 
                     delay_step_y,
                     alpha);

  ecg_obj.generate_12_lead_ecg_signals_one_cycle(ecg_result, 
                                                 x_result,
                                                 y_result, 
                                                 z_result,
                                                 v_result);
 /* ここまで */



 /* .txtファイルに書き込みして出力 ここから */
 std::string filename = "ecg_result.txt";
 std::ofstream outputfile(filename);
 for (int i = 0; i < step_num; i++) {
  for (int j = 0; j < ecg_num; j++) {
   outputfile << ecg_result[i][j] << "\t";
  }
  outputfile << "\n";
 }
 outputfile.close();

 std::string filename_x = "x_result.txt";
 std::ofstream outputfile_x(filename_x);
 for (int i = 0; i < step_num; i++) {
  for (int j = 0; j < vdp_variable_num; j++) {
   outputfile_x << x_result[i][j] << "\t";
  }
  outputfile_x << "\n";
 }
 outputfile_x.close();

 std::string filename_y = "y_result.txt";
 std::ofstream outputfile_y(filename_y);
 for (int i = 0; i < step_num; i++) {
  for (int j = 0; j < vdp_variable_num; j++) {
   outputfile_y << y_result[i][j] << "\t";
  }
  outputfile_y << "\n";
 }
 outputfile_y.close();

 std::string filename_z = "z_result.txt";
 std::ofstream outputfile_z(filename_z);
 for (int i = 0; i < step_num; i++) {
  for (int j = 0; j < fhn_variable_num; j++) {
   outputfile_z << z_result[i][j] << "\t";
  }
  outputfile_z << "\n";
 }
 outputfile_z.close();

 std::string filename_v = "v_result.txt";
 std::ofstream outputfile_v(filename_v);
 for (int i = 0; i < step_num; i++) {
  for (int j = 0; j < fhn_variable_num; j++) {
   outputfile_v << v_result[i][j] << "\t";
  }
  outputfile_v << "\n";
 }
 outputfile_v.close();



 /* ここまで */

 /* .csvファイルに書き込みして出力 ここから */
 // Open a file stream for writing
 std::ofstream file("ecg_result.csv");

 // Write the data to the file
 for (int i = 0; i < step_num; i++) {
  for (int j = 0; j < ecg_num; j++) {
   // Write the element to the file
   file << ecg_result[i][j];
   // Add a comma after the element, unless it's the last element in the row
   if (j < ecg_num - 1) {
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