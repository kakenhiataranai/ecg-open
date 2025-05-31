/* generate_12_lead_ecg.h */

#ifndef GENERATE_12_LEAD_ECG_H
#define GENERATE_12_LEAD_ECG_H


#include "rr_interval_using_fftw.h"
#include "math_random_number.h"

#include <vector>
#include <queue>
#include <cmath>



class van_der_pol_oscillator {

 public:

  van_der_pol_oscillator(std::vector<double> a, 
                         std::vector<double> f, 
                         std::vector<double> e,
                         std::vector<double> u,
                         std::vector<double> d,
                         std::vector<double> large_k);

  void modified_van_der_pol_oscillators_one_step_runge_kutta(std::vector<double>& y,
                                                             std::vector<double> y_last, 
                                                             std::vector<double> y_delay, 
                                                             std::vector<double> y_delay_interpolation, 
                                                             std::vector<double> y_delay_next, 
                                                             std::vector<double>& large_y_last,
                                                             std::vector<double>& large_y_h_1, 
                                                             std::vector<double>& large_y_h_2, 
                                                             std::vector<double>& large_y_h_3,
                                                             std::vector<double>& x,
                                                             std::vector<double> x_last,
                                                             double step_size, 
                                                             double f_1);
 private:
  
  int variable_num;
  std::vector<double> a;
  std::vector<double> f;
  std::vector<double> e;
  std::vector<double> u;
  std::vector<double> d;
  std::vector<double> large_k;

  double function_large_f(int i, double x);
  double function_large_g(int i, double x);
  double function_x_i(int i, double y);
  double function_y_i(int i, double x, double y, double y_delay);

};



class fitzhugh_nagumo_oscillator {

public:

 fitzhugh_nagumo_oscillator(std::vector<double> k, 
                            std::vector<double> c, 
                            std::vector<double> w_1,
                            std::vector<double> w_2, 
                            std::vector<double> b, 
                            std::vector<double> h, 
                            std::vector<double> g,
                            std::vector<double> large_c);

 void modified_fitzhugh_nagumo_oscillators_one_step_runge_kutta(std::vector<double>& z, 
                                                                std::vector<double> z_last, 
                                                                std::vector<double> large_y_last, 
                                                                std::vector<double> large_y_h_1, 
                                                                std::vector<double> large_y_h_2,
                                                                std::vector<double> large_y_h_3,
                                                                std::vector<double>& v, 
                                                                std::vector<double> v_last, 
                                                                double step_size);
private:

 int variable_num;
 std::vector<double> k;
 std::vector<double> c;
 std::vector<double> w_1;
 std::vector<double> w_2;
 std::vector<double> b;
 std::vector<double> h;
 std::vector<double> g;
 std::vector<double> large_c;

 int heaviside_step_function(double x);
 double function_z_j(int j, double z, double v, double large_y);
 double function_v_j(int j, double z, double v);

};


/*                                                                                                                            */
/* 12誘導心電図を生成するクラス                                                                                               */
/* Quiroz Juarez et al.(2018), Cardiac conduction model for generating 12 lead ECG signals with realistic heart rate dynamics */
/* に基づき実装           　　　                                                                                              */
/*                                                                                                                            */
/* Heart Rate Variability 式(4),(5) の逆フーリエ変換のためにフーリエ変換のライブラリ FFTW https://www.fftw.org/ を使用する    */
/*                                                                                                                            */
class ecg_signals {

public:

 ecg_signals(double ecg_step_size,
             int ecg_one_cycle_step_num, 
             std::vector<double> a,
             std::vector<double> f,
             std::vector<double> e,
             std::vector<double> u,
             std::vector<double> d,
             std::vector<double> large_k,
             std::vector<double> k,
             std::vector<double> c,
             std::vector<double> w_1,
             std::vector<double> w_2,
             std::vector<double> b,
             std::vector<double> h,
             std::vector<double> g,
             std::vector<double> large_c,
             double x_initial_value,
             double y_initial_value,
             double z_initial_value,
             double v_initial_value,
             int delay_step_y,
             std::vector<double> binomial_func_coef,
             std::vector<std::vector<double>> alpha,
             double respiratory_frequency_f_0,
             double respiratory_amplitude_z_0,
             int norm_dist_rand_seed,
             double norm_dist_rand_mean,
             double norm_dist_rand_standard_dev,
             int fft_num,
             int ecg_step_num_per_sec, 
             int bimodal_num,
             double hr_mean,
             double hr_std,
             int uni_dist_rand_seed,
             double uni_dist_rand_lower_bound,
             double uni_dist_rand_upper_bound,
             std::vector<double> f_r,
             std::vector<double> gamma,
             std::vector<double> sigma_square);

 void generate_12_lead_ecg_signals_one_cycle(std::vector<std::vector<double>>& z_result);
 int get_one_cycle_step_num() const;
 int get_ecg_num() const;

private:
 
 class van_der_pol_oscillator vdp_obj;
 class fitzhugh_nagumo_oscillator fn_obj;
 class rr_interval rr_interval_obj;
 class normal_dist_random_number normal_dist_rand_obj;  /* メルセンヌ・ツイスター  乱数シードは固定値 0 をセット             */
                                                        /* 正規分布に従う乱数  平均0 標準偏差 0.007[mv] に一様初期化         */
                                                        /* 論文では 7[mV] と書いてあるが 明らかに大きすぎるので 1/1000 する */

 double step_size;
 int one_cycle_step_num;                      /* 1サイクルあたりの計算ステップ数 */
 int solve_difference_equation_serial_index;  /* 通しの計算ステップ数            */
 int large_t_d_size;

 const int ecg_num = 12;  /* 12誘導の数は固定 */
 const int alpha_num = 4; /* alpha の数は固定 */
 const int x_num = 3;     /* xの数は固定 */
 const int y_num = 3;     /* yの数は固定 */
 const int z_num = 4;     /* zの数は固定 */
 const int v_num = 4;     /* vの数は固定 */

 int delay_step_y;        /* 時間遅れステップ数 論文では 0.092[s]  0.002 * 46 = 0.092 */

 std::vector<double> binomial_func_coef;

 double respiratory_frequency_f_0;  /* = 0.1 [Hz] 論文に書いてある値を使用 */
 double respiratory_amplitude_z_0;  /* = 0.015 [mV] 論文では 15[mV] と書いてあるが 明らかに大きすぎるので 1/1000 する */

 std::vector<std::vector<double>> alpha;

 std::vector<double> x_last;
 std::vector<double> y_last;
 std::vector<double> z_last;
 std::vector<double> v_last;

 std::vector<double> x_now;
 std::vector<double> y_now;
 std::vector<double> z_now;
 std::vector<double> v_now;

 std::vector<double> y_delay;
 std::vector<double> y_delay_interpolation;
 std::vector<double> y_delay_next;

 std::queue<double> queue_y_0_for_delay;
 std::queue<double> queue_y_1_for_delay;

 std::vector<double> large_t_d;

	
 double quadratic_function_phi(double f);  /* 式(7) */
 double compute_f_1(int index);

};



	
#endif