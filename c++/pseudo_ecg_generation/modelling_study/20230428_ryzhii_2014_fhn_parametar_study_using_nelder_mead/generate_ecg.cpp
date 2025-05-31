
#include "generate_ecg.h"


van_der_pol_oscillator::van_der_pol_oscillator(std::vector<double> a,
 std::vector<double> f,
 std::vector<double> e,
 std::vector<double> u,
 std::vector<double> d,
 std::vector<double> large_k) {
 this->a = a;
 this->f = f;
 this->e = e;
 this->large_k = large_k;
 this->u = u;
 this->d = d;
 this->variable_num = this->a.size();
}

double van_der_pol_oscillator::function_large_f(int i, double x) {
 double res = a[i] * (x * x - u[i]);
 return res;
}

double van_der_pol_oscillator::function_large_g(int i, double x) {
 double res = f[i] * x * (x + d[i]) * (x + e[i]);
 return res;
}

double van_der_pol_oscillator::function_x_i(int i, double y) {
 double res = y;
 return res;
}

double van_der_pol_oscillator::function_y_i(int i, double x, double y, double y_delay) {
 double res = -function_large_f(i, x) * y - function_large_g(i, x) + large_k[i] * (y_delay - y);
 return res;
}

void van_der_pol_oscillator::modified_van_der_pol_oscillators_one_step_runge_kutta(std::vector<double>& y,
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
 double step_size) {

 for (int i = 0; i < variable_num; i++) {
  double x_last_i = x_last[i];
  double y_last_i = y_last[i];

  /* ルンゲクッタ */
  double k_1 = step_size * function_x_i(i, y_last_i);
  double h_1 = step_size * function_y_i(i, x_last_i, y_last_i, y_delay[i]);
  double x_k_1 = x_last_i + 0.5 * k_1;
  double y_h_1 = y_last_i + 0.5 * h_1;

  double k_2 = step_size * function_x_i(i, y_h_1);
  double h_2 = step_size * function_y_i(i, x_k_1, y_h_1, y_delay_interpolation[i]);
  double x_k_2 = x_last_i + 0.5 * k_2;
  double y_h_2 = y_last_i + 0.5 * h_2;

  double k_3 = step_size * function_x_i(i, y_h_2);
  double h_3 = step_size * function_y_i(i, x_k_2, y_h_2, y_delay_interpolation[i]);
  double x_k_3 = x_last_i + k_3;
  double y_h_3 = y_last_i + h_3;

  double k_4 = step_size * function_x_i(i, y_h_3);
  double h_4 = step_size * function_y_i(i, x_k_3, y_h_3, y_delay_next[i]);

  x[i] = x_last_i + (k_1 + 2 * k_2 + 2 * k_3 + k_4) / 6;
  y[i] = y_last_i + (h_1 + 2 * h_2 + 2 * h_3 + h_4) / 6;

  if (i == 0 || i == 2) {
   large_y_last[i] = y_last_i;
   large_y_h_1[i] = y_h_1;
   large_y_h_2[i] = y_h_2;
   large_y_h_3[i] = y_h_3;
   large_y_last[i + 1] = (-1) * y_last_i;
   large_y_h_1[i + 1] = (-1) * y_h_1;
   large_y_h_2[i + 1] = (-1) * y_h_2;
   large_y_h_3[i + 1] = (-1) * y_h_3;
  }
 }
}



fitzhugh_nagumo_oscillator::fitzhugh_nagumo_oscillator(std::vector<double> k,
 std::vector<double> c,
 std::vector<double> w_1,
 std::vector<double> w_2,
 std::vector<double> b,
 std::vector<double> h,
 std::vector<double> g,
 std::vector<double> large_c) {
 this->k = k;
 this->c = c;
 this->w_1 = w_1;
 this->w_2 = w_2;
 this->b = b;
 this->h = h;
 this->g = g;

 /* 論文では                                                                                                    */
 /* large_c[0] = 4e-5; large_c[1] = -4e-5; large_c[2] = 9e-5; large_c[3] = -6e-5;                               */
 /* だが T波 が上下反転した結果となってしまう                                                                   */
 /* [21]Ryzhii and Ryzhii(2014)ではC_2とC_4に対応するものは正の数としているのでその通りに設定                   */
 /* large_c.resize(variable_num); large_c[0] = 4e-5; large_c[1] = 4e-5; large_c[2] = 9e-5; large_c[3] = 6e-5;   */
 this->large_c = large_c;

 this->variable_num = this->k.size();
}

int fitzhugh_nagumo_oscillator::heaviside_step_function(double x) {
 int res = 0;
 if (x > 0) {
  res = 1;
 }
 return res;
}

double fitzhugh_nagumo_oscillator::function_z_j(int j, double z, double v, double large_y) {
 double k_j = k[j];
 double c_j = c[j];
 double w_1_j = w_1[j];
 double w_2_j = w_2[j];
 double b_j = b[j];
 double g_j = g[j];
 double large_c_j = large_c[j];
 double res = k_j * ((-c_j) * z * (z - w_1_j) * (z - w_2_j) - b_j * v - g_j * v * z + large_c_j * large_y * (double)heaviside_step_function(large_y));
 return res;
}

double fitzhugh_nagumo_oscillator::function_v_j(int j, double z, double v) {
 double k_j = k[j];
 double h_j = h[j];
 double res = k_j * h_j * (z - v);
 return res;
}

void fitzhugh_nagumo_oscillator::modified_fitzhugh_nagumo_oscillators_one_step_runge_kutta(std::vector<double>& z,
 std::vector<double> z_last,
 std::vector<double> large_y_last,
 std::vector<double> large_y_h_1,
 std::vector<double> large_y_h_2,
 std::vector<double> large_y_h_3,
 std::vector<double>& v,
 std::vector<double> v_last,
 double step_size) {
 for (int j = 0; j < variable_num; j++) {
  double z_last_j = z_last[j];
  double v_last_j = v_last[j];
  double large_y_last_j = large_y_last[j];
  double large_y_h_1_j = large_y_h_1[j];
  double large_y_h_2_j = large_y_h_2[j];
  double large_y_h_3_j = large_y_h_3[j];

  /* ルンゲクッタ */
  double l_1 = step_size * function_z_j(j, z_last_j, v_last_j, large_y_last_j);
  double m_1 = step_size * function_v_j(j, z_last_j, v_last_j);
  double z_l_1 = z_last_j + 0.5 * l_1;
  double v_m_1 = v_last_j + 0.5 * m_1;

  double l_2 = step_size * function_z_j(j, z_l_1, v_m_1, large_y_h_1_j);
  double m_2 = step_size * function_v_j(j, z_l_1, v_m_1);
  double z_l_2 = z_last_j + 0.5 * l_2;
  double v_m_2 = v_last_j + 0.5 * m_2;

  double l_3 = step_size * function_z_j(j, z_l_2, v_m_2, large_y_h_2_j);
  double m_3 = step_size * function_v_j(j, z_l_2, v_m_2);
  double z_l_3 = z_last_j + l_3;
  double v_m_3 = v_last_j + m_3;

  double l_4 = step_size * function_z_j(j, z_l_3, v_m_3, large_y_h_3_j);
  double m_4 = step_size * function_v_j(j, z_l_3, v_m_3);

  z[j] = z_last_j + (l_1 + 2 * l_2 + 2 * l_3 + l_4) / 6;
  v[j] = v_last_j + (m_1 + 2 * m_2 + 2 * m_3 + m_4) / 6;
 }
}



/* モデルパラメータや初期値を意味するメンバ変数を初期化 */
ecg_signals::ecg_signals(double ecg_step_size,
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
 std::vector<std::vector<double>> alpha)

 : vdp_obj(a,
  f,
  e,
  u,
  d,
  large_k),

 fn_obj(k,
  c,
  w_1,
  w_2,
  b,
  h,
  g,
  large_c) {

 step_size = ecg_step_size;
 one_cycle_step_num = ecg_one_cycle_step_num;
 this->delay_step_y = delay_step_y;
 this->alpha = alpha;

 solve_difference_equation_serial_index = 0;

 x_last.assign(x_num, x_initial_value);   /* 論文に書いてある初期値をセット  */
 y_last.assign(y_num, y_initial_value);   /* 論文に書いてある初期値をセット  */
 z_last.assign(z_num, z_initial_value);   /* 論文に書いてある初期値をセット  */
 v_last.assign(v_num, v_initial_value);   /* 論文に書いてある初期値をセット  */

 x_now.resize(x_num);
 y_now.resize(y_num);
 z_now.resize(z_num);
 v_now.resize(v_num);

 y_delay.assign(y_num, y_initial_value);                  /* yのdelay項の初期値はyの初期値とする */
 y_delay_interpolation.assign(y_num, y_initial_value);    /* yのdelay項の初期値はyの初期値とする */
 y_delay_next.assign(y_num, y_initial_value);             /* yのdelay項の初期値はyの初期値とする */

}


void ecg_signals::generate_12_lead_ecg_signals_one_cycle(std::vector<std::vector<double>>& ecg_result,
 std::vector<std::vector<double>>& x_result,
 std::vector<std::vector<double>>& y_result,
 std::vector<std::vector<double>>& z_result,
 std::vector<std::vector<double>>& v_result) {



 for (int i = 0; i < one_cycle_step_num; i++) {

  if (delay_step_y <= solve_difference_equation_serial_index) {
   y_delay[1] = queue_y_0_for_delay.front(); queue_y_0_for_delay.pop(); /* y_delayは 1 ずれていてよい */
   y_delay[2] = queue_y_1_for_delay.front(); queue_y_1_for_delay.pop(); /* y_delayは 1 ずれていてよい */

   y_delay_next[1] = queue_y_0_for_delay.front();
   y_delay_next[2] = queue_y_1_for_delay.front();
   y_delay_interpolation[1] = 0.5 * (y_delay[1] + y_delay_next[1]);     /* とりあえず線形補間にしておく */
   y_delay_interpolation[2] = 0.5 * (y_delay[2] + y_delay_next[2]);     /* とりあえず線形補間にしておく */
  }

  std::vector<double> large_y_last(z_num);
  std::vector<double> large_y_h_1(z_num);
  std::vector<double> large_y_h_2(z_num);
  std::vector<double> large_y_h_3(z_num);

  /* 微分方程式を1ステップ計算実行 Runge–Kutta */
  this->vdp_obj.modified_van_der_pol_oscillators_one_step_runge_kutta(y_now,
   y_last,
   y_delay,
   y_delay_interpolation,
   y_delay_next,
   large_y_last,
   large_y_h_1,
   large_y_h_2,
   large_y_h_3,
   x_now,
   x_last,
   step_size);
  /* 微分方程式を1ステップ計算実行 Runge–Kutta */
  this->fn_obj.modified_fitzhugh_nagumo_oscillators_one_step_runge_kutta(z_now,
   z_last,
   large_y_last,
   large_y_h_1,
   large_y_h_2,
   large_y_h_3,
   v_now,
   v_last,
   step_size);

  //double ecg_base = 0.2;  /* z_0 = 0.2 と論文に書いてある */

  double ecg_base = 0;  /* z_0 = 0.2 と論文に書いてある */


  for (int j = 0; j < ecg_num; j++) {
   double ecg_tmp = ecg_base;
   ecg_tmp += alpha[j][0] * z_now[0];
   ecg_tmp -= alpha[j][1] * z_now[1]; /* マイナス */
   ecg_tmp += alpha[j][2] * z_now[2];
   ecg_tmp += alpha[j][3] * z_now[3];
   ecg_result[i][j] = ecg_tmp;
  }

  queue_y_0_for_delay.push(y_now[0]);
  queue_y_1_for_delay.push(y_now[1]);
  x_last = x_now;
  y_last = y_now;
  z_last = z_now;
  v_last = v_now;

  solve_difference_equation_serial_index++;


  /* 出力用の処理 */
  for (int j = 0; j < x_num; j++) {
   x_result[i][j] = x_now[j];
   y_result[i][j] = y_now[j];
  }
  for (int j = 0; j < z_num; j++) {
   z_result[i][j] = z_now[j];
   v_result[i][j] = v_now[j];
  }


 }
}