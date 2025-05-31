/* ecg_McSharry_model_using_fftw.h */

#ifndef ECG_MCSHARRY_MODEL_USING_FFTW_H
#define ECG_MCSHARRY_MODEL_USING_FFTW_H


#include "ecg_model_parameter.h"


/* case_1 debug */
#include <iostream>
#include <ostream>

#include <math.h>
#include <random>
#include <vector>
// #include <fftw3.h>    /* フーリエ変換のライブラリ FFTW   https://www.fftw.org/  */


//class rr_interval {
//
//public:
//
// rr_interval(double step_size_arg, int step_num_per_sec_arg);
// std::vector<double> large_t();    /* RR-intervals                                           */
//                                   /* 配列サイズは step_num_per_sec * fft_num                */
// std::vector<double> omega_t();    /* 回転角速度                                             */
//                                   /* 配列サイズは step_num_per_sec * fft_num                */
//
//private:
// 
//  const int fft_num = 10000;  /* FFTWの配列サイズ                         */
//                              /* 10000 [sec] / 3600 [sec] =  2.78 [hour]  */
//  
//  const int bimodal_num = 2;
//  //const int hr_mean = 70;     /* bpm (beat per minute) */
//  const int hr_mean = 62;     /* bpm (beat per minute) */
//  const int hr_std = 5;       /* bpm (beat per minute) */
//  
//  int step_num_per_sec;
//  double step_size;
//  std::vector<double> f_r;
//  std::vector<double> gamma;
//  std::vector<double> sigma_square;
//  double mean(std::vector<double> data);
//  double variance(std::vector<double> data);
//  double standard_deviation(std::vector<double> data);
//  double normal_distribution_probability_density(double x, double m, double sd);
//  double power_spectrum(double f);
//  double linear_interpolation(double x, double x_1, double x_2, double y_1, double y_2);
//};


/*                                                                                                             */
/* 心電図を生成するクラス                                                                                      */
/* McSharry et al.(2003), A dynamical model for generating synthetic electrocardiogram signals                 */
/* Tabassum and Ahmed (2020), A simplified cardiac conduction model and twelve-lead ECG generation             */
/*                                                                                                             */
/* に基づき実装           　　　                                                                               */
/*                                                                                                             */
class ecg_signals {

public:
 
 ecg_signals(struct ecg_model_parameter* ecg_model_parameter_pointer);

 void generate_ecg_signal_one_cycle(std::vector<std::vector<std::vector<double>>>& ecg_result);

private:
 
 // class rr_interval rr_interval_obj; 

 int ecg_num;                       /* 12誘導                */
 int mcsharry_variable_num;         /* x, y, z, omega の 4つ */
 int variable_num;                  /* x, y, z の 3つ        */
 int distinct_point_num;            /* P, Q, R, S, T の 5つ  */

 double step_size;                  /* [sec] */
 int step_num_per_sec;              /* = 1 / 0.002  */
 int one_cycle_step_num;            /* 1サイクルあたりの計算ステップ数 */

 std::vector<std::vector<double>> theta; /* channel_num × distinct_point_num */
 std::vector<std::vector<double>> a;     /* channel_num × distinct_point_num */
 std::vector<std::vector<double>> b;     /* channel_num × distinct_point_num */
 std::vector<std::vector<double>> alpha; /* channel_num × distinct_point_num */

 std::vector<std::vector<double>> u_initial;    /* ecg_num × variable_num */
 std::vector<std::vector<double>> u_last;       /* ecg_num × variable_num */
 std::vector<std::vector<double>> u_now;        /* ecg_num × variable_num */
 std::vector<double> omega_t;

 double x_initial;
 double y_initial;
 double z_initial;
 double omega_initial;
 int omega_t_size;

 double z_scalar;

 double large_a;
 double f_2;
 double omega;
 double t_now;
 
 int solve_differential_equation_serial_index;  /* 通しの計算ステップ数 */
 

 void mcsharry_model_one_step_runge_kutta(int channel, std::vector<double>& u, std::vector<double> u_last, double step_size, double omega, double t_now);

 std::vector<double> add_vector(std::vector<double> u_1, std::vector<double> u_2);
 std::vector<double> constant_product_vector(double scalar, std::vector<double> u);
 double derivative_z(int channel, double t, std::vector<double> u_last);
 std::vector<double> derivative_u(int channel, double t, std::vector<double> u_last);





 /* case_1 */
 bool disappear_cycle_flag;
 std::vector<int> disappear_distinct_point;





};

#endif