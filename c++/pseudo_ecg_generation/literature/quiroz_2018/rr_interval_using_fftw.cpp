
#include "rr_interval_using_fftw.h"

#include "math_random_number.h"
#include "math_basic_statistics.h"
#include "math_probability_distribution.h"
#include "math_interpolation.h"




rr_interval::rr_interval(int fft_num,
                         int step_num_per_sec, 
                         int bimodal_num,
                         double hr_mean,
                         double hr_std,
                         int mt_seed, 
                         double uniform_dist_lower_bound, 
                         double uniform_dist_upper_bound, 
                         std::vector<double> f_r, 
                         std::vector<double> gamma, 
                         std::vector<double> sigma_square) {
 
 this->fft_num = fft_num;
 this->step_num_per_sec = step_num_per_sec;
 this->bimodal_num = bimodal_num;
 this->hr_mean = hr_mean;
 this->hr_std = hr_std;
 this->mt_seed = mt_seed;
 this->uniform_dist_lower_bound = uniform_dist_lower_bound;
 this->uniform_dist_upper_bound = uniform_dist_upper_bound;
 this->f_r = f_r;
 this->gamma = gamma;
 this->sigma_square = sigma_square;

 /* sigma_square[0] / sigma_square[1] = 0.5 であれば                                              */
 /* sigma_square[0] と sigma_square[1] の値が何であれ計算結果である時系列 T_d の値は変わらないが  */
 /* McSharry et al., (2003) Fig.4. を概ね再現する値に設定してある                                 */
 //sigma_square.resize(bimodal_num); sigma_square[0] = 0.00325; sigma_square[1] = 0.0065;

}


/* Quiroz-Ju´arez et al., (2018)  式(4)         */
/* McSharry et al., (2003)  Fig.4. にグラフあり  */
double rr_interval::power_spectrum(double f) {
 normal_distribution normal_distribution_obj;
 double res = 0;
 for (int i = 0; i < bimodal_num; i++) {
  res += sigma_square[i] * normal_distribution_obj.probability_density(f, f_r[i], gamma[i]);
 }
 return res;
}


std::vector<double> rr_interval::large_t_d() {

/* FFTW 配列の宣言 */
 fftw_complex* in, * out;
 in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fft_num);
 out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fft_num);


 /* 乱数エンジン */
 uniform_dist_random_number uniform_obj(mt_seed, uniform_dist_lower_bound, uniform_dist_upper_bound);


 /* 複素数の列 CS(f) を作成                                                                                          */
 /* CS(f) is a sequence of complex numbers with amplitudes √S(f) and randomly distributed phases between 0 and 2π. */
 for (int i = 0; i < fft_num; i++) {

  /* 一様乱数 */
  double theta = uniform_obj.generate_random_number();

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


 basic_statistics basic_statistics_obj;
 double alpha_large_t = basic_statistics_obj.standard_deviation(large_t);


 double alpha = (double)60 / hr_mean;                          /* Quiroz - Ju´arez et al., (2018) 式(8) */
 double beta = (alpha / alpha_large_t) * (hr_std / hr_mean);   /* Quiroz - Ju´arez et al., (2018) 式(8) */

/* RR-intervals 1[sec]毎 */
 std::vector<double> v(fft_num);
 for (int i = 0; i < fft_num; i++) {
  v[i] = alpha + beta * large_t[i];
 }

 /* 1[sec]毎の RR-intervals のデータをステップサイズ(例えば 0.002 )[sec]毎のデータに変換  */
 linear_interpolation linear_interpolation_obj;
 double step_size = 1 / (double)step_num_per_sec;
 std::vector<double> w(step_num_per_sec * (fft_num - 1));
 int cnt = 0;
 for (int i = 1; i < fft_num; i++) {  
  double x_1 = i - 1;
  double x_2 = i;
  double y_1 = v[i - 1];
  double y_2 = v[i];
  for (int j = 0; j < step_num_per_sec; j++) {
   double x = x_1 + step_size * j;
   w[cnt] = linear_interpolation_obj.execute_linear_interpolation(x, x_1, x_2, y_1, y_2);
   ++cnt;
  }
 }

 return w;
}

