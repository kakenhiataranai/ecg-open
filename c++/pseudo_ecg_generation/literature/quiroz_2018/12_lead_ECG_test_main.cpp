#include <iostream>
#include <fstream>

#include "generate_12_lead_ecg.h"


#define PI 3.141592653589793


int main(){


 /* 計算パラメータ作成 ここから */
 double step_size = 0.002;   /* [sec] */
 int step_num = 5000;        /* シミュレーションステップ数  */

 int vdp_variable_num = 3;
 std::vector<double> a(vdp_variable_num); a[0] = 40; a[1] = 50; a[2] = 50;
 std::vector<double> f(vdp_variable_num); f[0] = 22; f[1] = 8.4; f[2] = 1.5;
 std::vector<double> e(vdp_variable_num); e[0] = 3.5; e[1] = 5; e[2] = 12;
 std::vector<double> u(vdp_variable_num); u[0] = 0.69; u[1] = 0.69; u[2] = 0.69;
 std::vector<double> d(vdp_variable_num); d[0] = 3; d[1] = 3; d[2] = 3;
 std::vector<double> large_k(vdp_variable_num); large_k[0] = 0; large_k[1] = 22; large_k[2] = 22;
 
 int fhn_variable_num = 4;
 std::vector<double> k(fhn_variable_num); k[0] = 2e3; k[1] = 1e2; k[2] = 1e4; k[3] = 2e3;
 std::vector<double> c(fhn_variable_num); c[0] = 0.26; c[1] = 0.12; c[2] = 0.12; c[3] = 0.1;
 std::vector<double> w_1(fhn_variable_num); w_1[0] = 0.13; w_1[1] = 0.12; w_1[2] = 0.12; w_1[3] = 0.22;
 std::vector<double> w_2(fhn_variable_num); w_2[0] = 1.0; w_2[1] = 1.1; w_2[2] = 1.1; w_2[3] = 0.8;
 std::vector<double> b(fhn_variable_num); b[0] = 0; b[1] = 0; b[2] = 0.015;  b[3] = 0;
 std::vector<double> h(fhn_variable_num); h[0] = 0.004; h[1] = 0.008; h[2] = 0.008; h[3] = 0.008;
 std::vector<double> g(fhn_variable_num); g[0] = 0.4; g[1] = 0.09; g[2] = 0.09; g[3] = 0.1;
 std::vector<double> large_c(fhn_variable_num); large_c[0] = 4e-5; large_c[1] = 4e-5; large_c[2] = 9e-5; large_c[3] = 6e-5;

 double x_initial_value = -0.1;
 double y_initial_value = 0.025;
 double z_initial_value = 0;
 double v_initial_value = 0;
 
 int delay_step_y = 46;      /* 時間遅れステップ数 論文では 0.092[s]  0.002 * 46 = 0.092 */
 
 int binomial_func_degree = 2;
 std::vector<double> binomial_func_coef(binomial_func_degree + 1); binomial_func_coef[0] = -0.7319; binomial_func_coef[1] = 8.798; binomial_func_coef[2] = 9.098;
 double respiratory_frequency_f_0 = 0.1;     /* = 0.1 [Hz] 論文に書いてある値を使用 */
 double respiratory_amplitude_z_0 = 0.015;   /* = 0.015 [mV] 論文では 15[mV] と書いてあるが 明らかに大きすぎるので 1/1000 する */

 int norm_dist_rand_seed = 0;
 double norm_dist_rand_mean = 0;             /* 正規分布に従う乱数  平均0 標準偏差 0.007[mv] に一様初期化 */
 double norm_dist_rand_standard_dev = 0.007; /* 論文では 7[mV] と書いてあるが 明らかに大きすぎるので 1/1000 する */

 int fft_num = 10000;
 int step_num_per_sec = 500; /* = 1 / 0.002  */

 int bimodal_num = 2;
 int hr_mean = 70;     /* bpm (beat per minute) */
 int hr_std = 5;  

 int uni_dist_rand_seed = 0;
 double uni_dist_rand_lower_bound = 0;
 double uni_dist_rand_upper_bound = 2 * PI;

 std::vector<double> f_r(bimodal_num); f_r[0] = 0.1; f_r[1] = 0.25;
 std::vector<double> gamma(bimodal_num); gamma[0] = 0.01; gamma[1] = 0.01;
 std::vector<double> sigma_square(bimodal_num); sigma_square[0] = 0.00325; sigma_square[1] = 0.0065;

 int ecg_num = 12;    /* 12誘導心電図 */
 

 std::vector<std::vector<double>> alpha(ecg_num, std::vector<double>(fhn_variable_num));

 /* Table Ⅰ */
alpha[0][0] = 0.5616;   alpha[0][1] = -0.03012; alpha[0][2] = 0.4969;  alpha[0][3] = 0.3964;
alpha[1][0] = 1.6768;   alpha[1][1] = -0.0602;  alpha[1][2] = 0.9941;  alpha[1][3] = 1.199;
alpha[2][0] = 1.1243;   alpha[2][1] = -0.03005; alpha[2][2] = 0.497;   alpha[2][3] = 0.801;

alpha[3][0] = -0.6483;  alpha[3][1] = 0.0367;   alpha[3][2] = -0.5964; alpha[3][3] = -0.7009;
alpha[4][0] = 0.8046;   alpha[4][1] = -0.0133;  alpha[4][2] = 0.1985;  alpha[4][3] =-0.397;
alpha[5][0] = 0.64455;  alpha[5][1] = -0.0478;  alpha[5][2] = 0.7949;  alpha[5][3] = 0.7008;

alpha[6][0] = 0.4565;   alpha[6][1] = 0.3979;   alpha[6][2] = -0.3981; alpha[6][3] = 0.2983;
alpha[7][0] = 0.1119;   alpha[7][1] = 1.288;    alpha[7][2] = -1.2508; alpha[7][3] = 2.9995;
alpha[8][0] = 1.3145;   alpha[8][1] = 1.2969;   alpha[8][2] = -0.3854;  alpha[8][3] = 1.803;

alpha[9][0] = 1.709;    alpha[9][1] = 1.2996;   alpha[9][2] = -0.1962;  alpha[9][3] = 1.2001;
alpha[10][0] = 1.5093;  alpha[10][1] = 1.1981;  alpha[10][2] = -0.0863; alpha[10][3] = 0.9036;
alpha[11][0] = 0.7485;  alpha[11][1] = -0.0367; alpha[11][2] = 0.7972;  alpha[11][3] = 0.40003;

/* Table Ⅲ */
 //alpha[0][0] = 1.9566;   alpha[0][1] = -0.0667; alpha[0][2] = 1.0781; alpha[0][3] = 1.5638;
 //alpha[1][0] = 1.5469;   alpha[1][1] = -0.0696; alpha[1][2] = 0.8705; alpha[1][3] = 1.0186;
 //alpha[2][0] = 0.5682;   alpha[2][1] = 0.0029; alpha[2][2] = 0.2037; alpha[2][3] = 0.1532;

 //alpha[3][0] = -0.7509;  alpha[3][1] = 0.0678; alpha[3][2] = -0.6281; alpha[3][3] = -0.7289;
 //alpha[4][0] = 0.2731;  alpha[4][1] = -0.0037; alpha[4][2] = 0.1026; alpha[4][3] = 0.3077;
 //alpha[5][0] = 1.6385;  alpha[5][1] = -0.0474; alpha[5][2] = 0.5982; alpha[5][3] = 0.7631;

 //alpha[6][0] = -0.0642;  alpha[6][1] = 0.0742;  alpha[6][2] = -0.4678; alpha[6][3] = -0.1019;
 //alpha[7][0] = 0.2769;  alpha[7][1] = 0.2411;  alpha[7][2] = -1.0371; alpha[7][3] = 1.4462;
 //alpha[8][0] = 0.9089;  alpha[8][1] = 0.5191;  alpha[8][2] = -0.7036; alpha[8][3] = 2.1496;

 //alpha[9][0] = 0.6677;   alpha[9][1] = 0.7796;  alpha[9][2] = -0.2363;  alpha[9][3] = 2.2085;
 //alpha[10][0] = 0.5761;  alpha[10][1] = 0.7216; alpha[10][2] = 0.00766; alpha[10][3] = 1.2517;
 //alpha[11][0] = 0.531;   alpha[11][1] = -0.0679; alpha[11][2] = 0.8745;  alpha[11][3] = 0.8225;


 std::vector<std::vector<double>> result(step_num, std::vector<double>(ecg_num));  /* サイズは (step_num × 12) */
 /* ここまで */


 /* 12誘導心電図の生成 計算実行 ここから */
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
                     binomial_func_coef,
                     alpha,
                     respiratory_frequency_f_0,
                     respiratory_amplitude_z_0,
                     norm_dist_rand_seed,
                     norm_dist_rand_mean,
                     norm_dist_rand_standard_dev,
                     fft_num, 
                     step_num_per_sec, 
                     bimodal_num, 
                     hr_mean, 
                     hr_std,
                     uni_dist_rand_seed,
                     uni_dist_rand_lower_bound,
                     uni_dist_rand_upper_bound,
                     f_r,
                     gamma,
                     sigma_square);

 ecg_obj.generate_12_lead_ecg_signals_one_cycle(result);
 /* ここまで */


 /* .txtファイルに書き込みして出力 ここから */
 std::string filename = "ecg_result.txt";
 std::ofstream outputfile(filename);
 for (int i = 0; i < step_num; i++) {
  for (int j = 0; j < ecg_num; j++) {
   outputfile << result[i][j] << "\t";
  }
  outputfile << "\n";
 }
 outputfile.close();
 /* ここまで */



 /* .csvファイルに書き込みして出力 ここから */
 // Open a file stream for writing
 std::ofstream file("ecg_result.csv");

 // Write the data to the file
 for (int i = 0; i < step_num; i++) {
  for (int j = 0; j < ecg_num; j++) {
   // Write the element to the file
   file << result[i][j];
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