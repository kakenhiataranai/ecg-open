/* ecg_McSharry_model_using_fftw.h */

#ifndef ECG_MCSHARRY_MODEL_USING_FFTW_H
#define ECG_MCSHARRY_MODEL_USING_FFTWH


#include <math.h>
#include <random>
#include <vector>
#include <fftw3.h>    /* フーリエ変換のライブラリ FFTW   https://www.fftw.org/  */


class rr_interval {

public:

	rr_interval(double step_size_arg, int step_num_per_sec_arg);
	std::vector<double> large_t();    /* RR-intervals                                           */
                                   /* 配列サイズは step_num_per_sec * fft_num                */

	std::vector<double> omega_t();    /* 回転角速度                                             */
                                   /* 配列サイズは step_num_per_sec * fft_num                */


private:

	const int fft_num = 10000;  /* FFTWの配列サイズ                         */
	/* 10000 [sec] / 3600 [sec] =  2.78 [hour]  */

	const int bimodal_num = 2;
	//const int hr_mean = 70;     /* bpm (beat per minute) */
	const int hr_mean = 65;     /* bpm (beat per minute) */
	const int hr_std = 5;       /* bpm (beat per minute) */

	int step_num_per_sec;
	double step_size;

	std::vector<double> f_r;
	std::vector<double> gamma;
	std::vector<double> sigma_square;

	double mean(std::vector<double> data);
	double variance(std::vector<double> data);
	double standard_deviation(std::vector<double> data);
	double normal_distribution_probability_density(double x, double m, double sd);

	double power_spectrum(double f);
	double linear_interpolation(double x, double x_1, double x_2, double y_1, double y_2);

};



class mcsharry_model {

public:
	mcsharry_model();

	void mcsharry_model_one_step_runge_kutta(std::vector<double>& u, std::vector<double> u_last, double step_size, double omega, double t);

private:

	int variable_num = 3;         /* u = (x,y,z) */
	int distinct_points_num = 5;  /* R,S,T,P,Q */
	double omega;
	double large_a;
	double f_2;
	double t_now;
	std::vector<double> theta;
	std::vector<double> a;
	std::vector<double> b;	


	std::vector<double> add_vector(std::vector<double> u_1, std::vector<double> u_2);
	std::vector<double> constant_product_vector(double scalar, std::vector<double> u);

	double derivative_z(double t, std::vector<double> u_last);
	std::vector<double> derivative_u(double t, std::vector<double> u_last);

};


/*                                                                                                             */
/* 心電図を生成するクラス                                                                                      */
/* McSharry et al.(2019), A dynamical model for generating synthetic electrocardiogram signals                 */
/* に基づき実装           　　　                                                                               */
/* class mcsharry_model  が必要                                                                                */
/*                                                                                                             */
class ecg_signals {

public:

	ecg_signals(double ecg_step_size, int ecg_one_cycle_step_num, int ecg_step_num_per_sec);
	void generate_ecg_signal_one_cycle(std::vector<std::vector<double>>& ecg_result);

private:

	class mcsharry_model mcsharry_obj;
	class rr_interval rr_interval_obj;
	
	double step_size;
	int one_cycle_step_num;                        /* 1サイクルあたりの計算ステップ数 */
	int solve_differential_equation_serial_index;  /* 通しの計算ステップ数            */
	int omega_t_size;

	int variable_num = 4; /* x, y, z, omega の4つ */

	std::vector<double> u_initial;
	std::vector<double> u_last;
	std::vector<double> u_now;

	std::vector<double> omega_t;

};


#endif