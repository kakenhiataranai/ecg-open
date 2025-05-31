/* rr_interval_Thuraisingham_model.h */

#ifndef RR_INTERVAL_THURAISINGHAM_MODEL_H
#define RR_INTERVAL_THURAISINGHAM_MODEL_H


#include <vector>
#include <queue>


class Thuraisingham_model {

public:
	Thuraisingham_model(double step_size_arg, int one_cycle_step_num_arg, int step_num_per_sec_arg, double beat_frequency_in_bpm_arg, int large_g_t_term_in_sec_arg, double large_c_initial_arg, double large_a_arg);
	
	void Thuraisingham_model_one_step_runge_kutta(double& x_now, double x_last, double& y_now, double y_last, double step_size, double large_g_t_now, double large_g_t_last, double large_g_t_interpolated);
	
	double linear_interpolation(double x, double x_1, double x_2, double y_1, double y_2);

	std::vector<double> large_g_t;

private:
	double step_size;
	int one_cycle_step_num;
	int step_num_per_sec;
	int large_g_t_term_in_sec;

	//double beat_frequency_in_bpm;
	double a_1 = 40;
	double u_11 = 0.83;
	double u_12 = -0.83;
	double d_1 = 3;
	double e_1 = 3.5;
	double f_1;
	double f_1_function(double beat_frequency_in_bpm);
	double derivative_x(double y);
	double derivative_y(double x, double y);
		
	double large_c_initial;
	double large_a;	
	std::vector<double> compute_large_g_t();
	double improved_logistic_map(double x);
};


/*                                                                                                             */
/* 洞房結節の活動電位とRR間隔を生成するクラス                                                                  */
/* Thuraisingham (2022), Generation of synthetic RR interval sequences of healthy patients                     */
/* に基づき実装           　　　                                                                               */
/* class Thuraisingham_model が必要                                                                            */
/*                                                                                                             */
class sinoatrial_node_action_potential {

public:

	sinoatrial_node_action_potential(double step_size_arg, int one_cycle_step_num_arg, int step_num_per_sec_arg, double beat_frequency_in_bpm_arg, int large_g_t_term_in_sec_arg, double large_c_initial_arg, double large_a_arg);
	void generate_sinoatrial_node_action_potential_one_cycle(std::vector<std::vector<double>>& sinoatrial_node_result, std::vector<double>& rr_interval_result);

private:

	class Thuraisingham_model thuraisingham_model_obj;
	
	double step_size;
	int one_cycle_step_num;
	int solve_differential_equation_serial_index;	
	
	double x_initial;
	double y_initial;
	double x_last;
	double y_last;
	double x_now;
	double y_now;

	std::vector<double> large_g_t;
	int large_g_t_size;
};


#endif