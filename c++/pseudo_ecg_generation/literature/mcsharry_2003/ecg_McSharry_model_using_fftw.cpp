
#include "ecg_McSharry_model_using_fftw.h"


#define PI 3.1415926535897932384626433832795028841971


/* パラメータ設定 */
/* sigma_square[0] / sigma_square[1] = 0.5 であれば                                              */
/* sigma_square[0] と sigma_square[1] の値が何であれ計算結果である時系列 T の値は変わらないが  */
/* McSharry et al., (2003) Fig.4. を概ね再現する値に設定してある                                 */
rr_interval::rr_interval(double step_size_arg, int step_num_per_sec_arg) {
 step_num_per_sec = step_num_per_sec_arg;
 f_r.resize(bimodal_num); f_r[0] = 0.1; f_r[1] = 0.25;
 gamma.resize(bimodal_num); gamma[0] = 0.01; gamma[1] = 0.01;
 sigma_square.resize(bimodal_num); sigma_square[0] = 0.00325; sigma_square[1] = 0.0065;
 step_size = step_size_arg;
}

/* 平均 */
double rr_interval::mean(std::vector<double> data) {
 double res = 0;
 int num = data.size();
 for (int i = 0; i < num; i++) {
  res += data[i];
 }
 res /= num;
 return res;
}

/* 分散 */
double rr_interval::variance(std::vector<double> data) {
 double res = 0;
 double m = mean(data);
 int num = data.size();
 for (int i = 0; i < num; i++) {
  res += data[i] * data[i];
 }
 res /= num;
 res -= m * m;
 return res;
}

/* 標準偏差 */
double rr_interval::standard_deviation(std::vector<double> data) {
 double res = sqrt(this->variance(data));
 return res;
}

/* 正規分布の密度関数 */
double rr_interval::normal_distribution_probability_density(double x, double m, double sd) {
 double res;
 res = exp(-0.5 * (x - m) * (x - m) / (sd * sd)) / (sd * sqrt(2 * PI));
 return res;
}

/* Quiroz-Ju´arez et al., (2018)  式(4)         */
/* McSharry et al., (2003)  Fig.4. にグラフあり  */
double rr_interval::power_spectrum(double f) {
 double res = 0;
 for (int i = 0; i < 2; i++) {
  res += sigma_square[i] * normal_distribution_probability_density(f, f_r[i], gamma[i]);
 }
 return res;
}

double rr_interval::linear_interpolation(double x, double x_1, double x_2, double y_1, double y_2) {
 double res = y_1;
 res += (y_2 - y_1) / (x_2 - x_1) * (x - x_1);
 return res;
}

std::vector<double> rr_interval::large_t() {

 /* FFTW 配列の宣言                                */
 fftw_complex* in, * out;
 in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fft_num);
 out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fft_num);


 /* 乱数エンジン */
 std::mt19937_64  engine(0);
 std::uniform_real_distribution<> dist(0, 2 * PI);


 /* 複素数の列 CS(f) を作成                                                                                          */
 /* CS(f) is a sequence of complex numbers with amplitudes √S(f) and randomly distributed phases between 0 and 2π. */
 for (int i = 0; i < fft_num; i++) {
  double theta = dist(engine); /* 一様乱数 */
  double amplitude = sqrt(power_spectrum((double)i / (double)fft_num));
  out[i][0] = amplitude * cos(theta);
  out[i][1] = amplitude * sin(theta);
 }


 /* FFTW 逆フーリエ変換 */
 fftw_plan plan_backward;
 plan_backward = fftw_plan_dft_1d(fft_num, out, in, FFTW_BACKWARD, FFTW_ESTIMATE);
 fftw_execute(plan_backward);

 /* 実部を取り出す */
 std::vector<double> large_t(fft_num);
 for (int i = 0; i < fft_num; i++) {
  large_t[i] = in[i][0];
 }

 /* FFTW 解放 */
 fftw_destroy_plan(plan_backward);
 fftw_free(out);
 fftw_free(in);


 double alpha_large_t = standard_deviation(large_t);

 double alpha = (double)60 / (double)hr_mean;                                  /* Quiroz - Ju´arez et al., (2018) 式(8) */
 double beta = (alpha / alpha_large_t) * ((double)hr_std / (double)hr_mean);   /* Quiroz - Ju´arez et al., (2018) 式(8) */


 /* RR-intervals 1[sec]毎 */
 std::vector<double> v(fft_num);
 for (int i = 0; i < fft_num; i++) {
  v[i] = alpha + beta * large_t[i];
 }


 /* 1[sec]毎の T(t) のデータをステップサイズ(例えば 0.002 )[sec]毎のデータに変換  */
 std::vector<double> w(step_num_per_sec * fft_num);
 int cnt = 0;
 for (int i = 1; i < fft_num; i++) {
  double x_1 = i - 1;
  double x_2 = i;
  double y_1 = v[i - 1];
  double y_2 = v[i];
  for (int j = 0; j < step_num_per_sec; j++) {
   double x = x_1 + step_size * j;
   w[cnt] = linear_interpolation(x, x_1, x_2, y_1, y_2);
   ++cnt;
  }
 }

 return w;
}

std::vector<double> rr_interval::omega_t() {
 std::vector<double> tmp;
 tmp = this->large_t();
 int num = tmp.size();
 std::vector<double> res(num);
 for (int i = 0; i < num; i++) {
  res[i] = 2 * PI / tmp[i];
 }
 return res;
}


mcsharry_model::mcsharry_model() {  /* モデルパラメータを意味するメンバ変数を初期化 */

 theta.assign(distinct_points_num, 0);
 a.assign(distinct_points_num, 0);
 b.assign(distinct_points_num, 0);

 /* R,S,T,P,Q の順に設定 */
 theta[0] = 0; theta[1] = PI / 12; theta[2] = PI / 2; theta[3] = -PI / 3; theta[4] = -PI / 12;
 a[0] = 30.0; a[1] = -7.5; a[2] = 0.75; a[3] = 1.2; a[4] = -5.0;
 b[0] = 0.1; b[1] = 0.1; b[2] = 0.4; b[3] = 0.25; b[4] = 0.1;

 large_a = 0.15 * 0.001;  /*  論文では 0.15[mV] と書いてあるが 明らかに大きすぎるので 1/1000 する */
 f_2 = 0.25;

 omega = 2 * PI;   /* この初期値は結果に影響しない */
 t_now = 0;        /* この初期値は結果に影響しない */

}

std::vector<double> mcsharry_model::add_vector(std::vector<double> u_1, std::vector<double> u_2) {
 int num = u_1.size();
 std::vector<double> res(num, 0);
 for (int i = 0; i < num; i++) {
  res[i] = u_1[i] + u_2[i];
 }
 return res;
}

std::vector<double> mcsharry_model::constant_product_vector(double scalar, std::vector<double> u) {
 int num = u.size();
 std::vector<double> res(num, 0);
 res = u;
 for (int i = 0; i < num; i++) {
  res[i] *= scalar;
 }
 return res;
}

double mcsharry_model::derivative_z(double t, std::vector<double> u) {
 double res = 0;
 double theta_last = atan2(u[1], u[0]);
 for (int i = 0; i < distinct_points_num; i++) { /* R,S,T,P,Q */
  double delta_theta = theta_last - theta[i];
  res += a[i] * delta_theta * exp((-1) * delta_theta * delta_theta / (2 * b[i] * b[i]));
 }
 res += u[2];
 double z_0 = large_a * sin(2 * PI * f_2 * t);
 res -= z_0;
 res *= (-1);
 return res;
}

std::vector<double> mcsharry_model::derivative_u(double t, std::vector<double> u) {
 int num = u.size();
 std::vector<double> res(num, 0);
 double alpha = 1 - sqrt(u[0] * u[0] + u[1] * u[1]);
 res[0] = alpha * u[0] - omega * u[1];
 res[1] = alpha * u[1] + omega * u[0];
 res[2] = derivative_z(t, u);
 return res;
}

void mcsharry_model::mcsharry_model_one_step_runge_kutta(std::vector<double>& u, std::vector<double> u_last, double step_size, double omega_arg, double t_arg) {

 omega = omega_arg;
 t_now = t_arg;

 std::vector<double> k_1(variable_num);
 std::vector<double> k_2(variable_num);
 std::vector<double> k_3(variable_num);
 std::vector<double> k_4(variable_num);

 /* ルンゲクッタ開始 */
 k_1 = derivative_u(t_now, u_last);
 k_2 = derivative_u(t_now + 0.5 * step_size, add_vector(u_last, constant_product_vector(step_size * 0.5, k_1)));
 k_3 = derivative_u(t_now + 0.5 * step_size, add_vector(u_last, constant_product_vector(step_size * 0.5, k_2)));
 k_4 = derivative_u(t_now + step_size, add_vector(u_last, constant_product_vector(step_size, k_3)));

 std::vector<double> k_1_2(variable_num);
 std::vector<double> k_3_4(variable_num);
 k_1_2 = add_vector(k_1, constant_product_vector(2, k_2));
 k_3_4 = add_vector(k_4, constant_product_vector(2, k_3));

 double scalar = step_size; scalar /= 6;
 u = add_vector(u_last, constant_product_vector(scalar, add_vector(k_1_2, k_3_4)));
}



/* モデルパラメータや初期値を意味するメンバ変数を初期化 */
ecg_signals::ecg_signals(double ecg_step_size, int ecg_one_cycle_step_num, int step_num_per_sec) : rr_interval_obj(ecg_step_size, step_num_per_sec) {
 //ecg_signals::ecg_signals(double ecg_step_size, int ecg_one_cycle_step_num, int step_num_per_sec) {

 step_size = ecg_step_size;
 one_cycle_step_num = ecg_one_cycle_step_num;
 solve_differential_equation_serial_index = 0;

 this->omega_t = this->rr_interval_obj.omega_t();
 omega_t_size = omega_t.size();
 //omega_t_size = 500 * 10000;
 //omega_t.assign(omega_t_size, 2 * PI);

 u_initial.assign(variable_num, 0);  /* 論文の図から読み取れる初期値をセット         */
 u_initial[0] = 1;                   /* 論文の図から読み取れる初期値をセット         */
 u_initial[2] = 0.04;                /* すぐに収束するなんとなくいい値を選んでセット */
 u_last = u_initial;
}

void ecg_signals::generate_ecg_signal_one_cycle(std::vector<std::vector<double>>& ecg_result) {

 for (int i = 0; i < one_cycle_step_num; i++) {

  double t_now = (solve_differential_equation_serial_index + 1) * step_size;

  int omega_t_index = solve_differential_equation_serial_index % omega_t_size;
  double omega_last = omega_t[omega_t_index];

  /* 微分方程式を1ステップ計算実行 Runge–Kutta */
  this->mcsharry_obj.mcsharry_model_one_step_runge_kutta(u_now, u_last, step_size, omega_last, t_now);

  ecg_result[i][0] = t_now;
  ecg_result[i][1] = u_now[0];      /* x                     */
  ecg_result[i][2] = u_now[1];      /* y                     */
  ecg_result[i][3] = 25 * u_now[2]; /* z : 25を乗じて波形の高さを論文の結果くらいになるように調整  */
  ecg_result[i][4] = omega_t[omega_t_index + 1];
  u_last = u_now;

  solve_differential_equation_serial_index++;
 }
}