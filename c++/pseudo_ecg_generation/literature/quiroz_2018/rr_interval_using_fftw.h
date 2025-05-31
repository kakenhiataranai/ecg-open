/* rr_interval_using_fftw.h */

#ifndef RR_INTERVAL_USING_FFTW_H
#define RR_INTERVAL_USING_FFTW_H


#include <math.h>
#include <random>
#include <vector>
#include <fftw3.h>    /* 高速フーリエ変換のライブラリ FFTW   https://www.fftw.org/  */




class rr_interval {

 public:		
  
  rr_interval(int fft_num,
              int step_num_per_sec, 
              int bimodal_num,
              double hr_mean,
              double hr_std,
              int mt_seed, 
              double uniform_dist_lower_bound, 
              double uniform_dist_upper_bound, 
              std::vector<double> f_r,
              std::vector<double> gamma,
              std::vector<double> sigma_square);
  
  std::vector<double> large_t_d();    /* RR-intervals                                          */
                                      /* Quiroz-Ju´arez et al., (2018) Fig.3.(b) にグラフあり */
                                      /* 一様乱数が違うので全く同じものではないが類似している  */
                                      /*                                                       */
                                      /* 配列サイズは step_num_per_sec * fft_num               */

 private:

  int fft_num;          /* FFTWの配列サイズ                         */
                        /* 10000 [sec] / 3600 [sec] =  2.78 [hour]  */

  int step_num_per_sec;

  int bimodal_num;
  double hr_mean;   /* bpm (beat per minute) */
  double hr_std;    /* bpm (beat per minute) */

  int mt_seed;
  double uniform_dist_lower_bound;
  double uniform_dist_upper_bound;  
  
  std::vector<double> f_r;
  std::vector<double> gamma;
  std::vector<double> sigma_square;

  double power_spectrum(double f);

};

	
#endif