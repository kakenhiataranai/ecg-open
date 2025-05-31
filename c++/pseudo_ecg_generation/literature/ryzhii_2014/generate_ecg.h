/* generate_12_lead_ecg.h */

#ifndef GENERATE_12_LEAD_ECG_H
#define GENERATE_12_LEAD_ECG_H


#include <vector>
#include <queue>



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
                                                             double step_size);
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


/*                                                                                                  */
/* 心電図を生成するクラス                                                                           */
/*                                                                                                  */
/* Ryzhii and  Ryzhii(2014), A heterogeneous coupled oscillator model for simulation of ECG signals */
/* に基づき実装           　　　                                                                    */
/*                                                                                                  */
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
             std::vector<std::vector<double>> alpha);


 void generate_12_lead_ecg_signals_one_cycle(std::vector<std::vector<double>>& ecg_result,
                                             std::vector<std::vector<double>>& x_result,
                                             std::vector<std::vector<double>>& y_result,
                                             std::vector<std::vector<double>>& z_result,
                                             std::vector<std::vector<double>>& v_result);


private:
 
 class van_der_pol_oscillator vdp_obj;
 class fitzhugh_nagumo_oscillator fn_obj;


 double step_size;
 int one_cycle_step_num;                      /* 1サイクルあたりの計算ステップ数 */
 int solve_difference_equation_serial_index;  /* 通しの計算ステップ数            */

 const int ecg_num = 12;  /* 12誘導の数は固定 */
 const int alpha_num = 4; /* alpha の数は固定 */
 const int x_num = 3;     /* xの数は固定 */
 const int y_num = 3;     /* yの数は固定 */
 const int z_num = 4;     /* zの数は固定 */
 const int v_num = 4;     /* vの数は固定 */

 int delay_step_y;        /* 時間遅れステップ数 論文では 0.092[s]  0.002 * 46 = 0.092 */

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

};



	
#endif