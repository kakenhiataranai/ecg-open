/* ecg_BVAM_model.h */

#ifndef ECG_BVAM_MODEL_H
#define ECG_BVAM_MODEL_H


#include <vector>





class bvam_model {

public:
	bvam_model(double large_c, double large_h, double beta, double gamma_t);

	void bvam_model_one_step_runge_kutta(std::vector<double>& x, std::vector<double> x_last, double step_size);

private:

	int variable_num = 3;
	double large_c;
	double large_h;
	double beta;
	double gamma_t;

	std::vector<double> add_vector(std::vector<double> x_1, std::vector<double> x_2);
	std::vector<double> constant_product_vector(double scalar, std::vector<double> x);
	std::vector<double> derivative_x(std::vector<double> x);

};


/*                                                                                                             */
/* 心電図を生成するクラス                                                                                      */
/* Quiroz Juarez et al.(2019), Generation of ECG signals from a reaction-diffusion model spatially discretized */
/* に基づき実装           　　　                                                                               */
/* class bvam_model  が必要                                                                                    */
/*                                                                                                             */
class ecg_signals {

public:

	ecg_signals(double ecg_step_size, int ecg_one_cycle_step_num, double large_c, double large_h, double beta, double gamma_t, std::vector<double> alpha);
	void generate_ecg_signal_one_cycle(std::vector<double>& ecg_result, std::vector<std::vector<double>>& x_result);

private:

	class bvam_model bvam_obj;
	
	double step_size;
	int one_cycle_step_num;

	const int alpha_num = 4; /* alpha の数は固定 */
	const int x_num = 4;     /* xの数は固定 */
 std::vector<double> alpha;
	std::vector<double> x_initial;
	std::vector<double> x_last;
	std::vector<double> x_now;
};


#endif