
#include "rr_interval_Thuraisingham_model.h"



Thuraisingham_model::Thuraisingham_model(double step_size_arg, int one_cycle_step_num_arg, int step_num_per_sec_arg, double beat_frequency_in_bpm_arg, int large_g_t_term_in_sec_arg, double large_c_initial_arg, double large_a_arg) {  /* モデルパラメータを意味するメンバ変数を初期化 */

	step_size = step_size_arg;
	one_cycle_step_num = one_cycle_step_num_arg;
	step_num_per_sec = step_num_per_sec_arg;
	large_g_t_term_in_sec = large_g_t_term_in_sec_arg;
	large_c_initial = large_c_initial_arg;
	large_a = large_a_arg;

	this->f_1 = this->f_1_function(beat_frequency_in_bpm_arg);
	this->large_g_t = this->compute_large_g_t();
}

double Thuraisingham_model::derivative_x(double y) {
	double res = y;
	return res;
}

double Thuraisingham_model::derivative_y(double x, double y) {
	double res = -a_1 * y * (x - u_11) * (x - u_12) - f_1 * x * (x + d_1) * (x + e_1);
	return res;
}

void Thuraisingham_model::Thuraisingham_model_one_step_runge_kutta(double& x_now, double x_last, double& y_now, double y_last, double step_size, double large_g_t_now, double large_g_t_last, double large_g_t_interpolated) {

	/* ルンゲクッタ */
	double k_1 = step_size * derivative_x(y_last);
	double h_1 = step_size * (derivative_y(x_last, y_last) + large_g_t_last);
	double x_k_1 = x_last + 0.5 * k_1;
	double y_h_1 = y_last + 0.5 * h_1;

	double k_2 = step_size * derivative_x(y_h_1);
	double h_2 = step_size * (derivative_y(x_k_1, y_h_1) + large_g_t_interpolated);
	double x_k_2 = x_last + 0.5 * k_2;
	double y_h_2 = y_last + 0.5 * h_2;

	double k_3 = step_size * derivative_x(y_h_2);
	double h_3 = step_size * (derivative_y(x_k_2, y_h_2) + large_g_t_interpolated);
	double x_k_3 = x_last + k_3;
	double y_h_3 = y_last + h_3;

	double k_4 = step_size * derivative_x(y_h_3);
	double h_4 = step_size * (derivative_y(x_k_3, y_h_3) + large_g_t_now);

	x_now = x_last + (k_1 + 2 * k_2 + 2 * k_3 + k_4) / 6;
	y_now = y_last + (h_1 + 2 * h_2 + 2 * h_3 + h_4) / 6;
}

double Thuraisingham_model::f_1_function(double beat_frequency_in_bpm) {
	double b = beat_frequency_in_bpm;
	double res = 0;	
	res = 0.0026 * b * b + 0.1344 * b - 0.3579;
	return res;
}

double Thuraisingham_model::improved_logistic_map(double x) {
	double res = 1 - 2 * x * x;
	return res;
}

double Thuraisingham_model::linear_interpolation(double x, double x_1, double x_2, double y_1, double y_2) {
	double res = y_1;
	res += (y_2 - y_1) / (x_2 - x_1) * (x - x_1);
	return res;
}

std::vector<double> Thuraisingham_model::compute_large_g_t() {

	//int v_size = one_cycle_step_num * step_size * 2;
	int v_size = 2 * large_g_t_term_in_sec;  /* 0.5[sec]毎なので2倍 */

	std::vector<double> v(v_size, 0);

	/* 0.5[sec]毎の時系列を生成 */
	v[0] = large_c_initial;
	for (int i = 1; i < v_size; i++) {		   /* i = 1 から */
		v[i] = improved_logistic_map(v[i - 1]);
	}

	/* G(t) に変換 */
	for (int i = 0; i < v_size; i++) {
		v[i] *= large_a;
	}

	/* 0.5[sec]毎の v(t) のデータをステップサイズ(例えば 0.002 )[sec]毎のデータに変換  */
	std::vector<double> w(step_num_per_sec * large_g_t_term_in_sec);
	int cnt = 0;
	for (int i = 1; i < v_size; i++) {
		double x_1 = i - 1;
		double x_2 = i;
		double y_1 = v[i - 1];
		double y_2 = v[i];
		for (int j = 0; j < step_num_per_sec / 2; j++) {
			double x = x_1 + step_size * j;
			w[cnt] = linear_interpolation(x, x_1, x_2, y_1, y_2);
			++cnt;
		}
	}
	return w;
}

/* モデルパラメータや初期値を意味するメンバ変数を初期化 */
sinoatrial_node_action_potential::sinoatrial_node_action_potential(double step_size_arg, int one_cycle_step_num_arg, int step_num_per_sec_arg, double beat_frequency_in_bpm_arg, int large_g_t_term_in_sec_arg, double large_c_initial_arg, double large_a_arg) : thuraisingham_model_obj(step_size_arg, one_cycle_step_num_arg, step_num_per_sec_arg, beat_frequency_in_bpm_arg, large_g_t_term_in_sec_arg, large_c_initial_arg, large_a_arg) {

	step_size = step_size_arg;
	one_cycle_step_num = one_cycle_step_num_arg;
	solve_differential_equation_serial_index = 0;

	x_initial = -0.1;      /* 論文に書いてある初期値をセット */
	y_initial = 0.025;     /* 論文に書いてある初期値をセット */
	x_last = x_initial;
	y_last = y_initial;

	this->large_g_t = this->thuraisingham_model_obj.large_g_t;
	large_g_t_size = large_g_t.size();
}

void sinoatrial_node_action_potential::generate_sinoatrial_node_action_potential_one_cycle(std::vector<std::vector<double>>& sinoatrial_node_result, std::vector<double>& rr_interval_result) {

	/* rr間隔生成用 */
	std::queue<double> t_at_y_equal_zero;

	for (int i = 0; i < one_cycle_step_num; i++) {

		double t_now = (solve_differential_equation_serial_index + 1) * step_size;

		int large_g_t_index = solve_differential_equation_serial_index % large_g_t_size;
		double large_g_t_last = 0;
		double large_g_t_now = 0;
		double large_g_t_interpolated = 0;	
		
		if (large_g_t_index != large_g_t_size - 1) {
			double t_last = (solve_differential_equation_serial_index) * step_size;
			double t_interpolated = t_now + 0.5 * step_size;
			large_g_t_last = large_g_t[large_g_t_index];
			large_g_t_now = large_g_t[large_g_t_index + 1];
			large_g_t_interpolated = thuraisingham_model_obj.linear_interpolation(t_interpolated, t_last, t_now, large_g_t_last, large_g_t_now);
		}

		
		/* 微分方程式を1ステップ計算実行 Runge–Kutta */
		this->thuraisingham_model_obj.Thuraisingham_model_one_step_runge_kutta(x_now, x_last, y_now, y_last, step_size, large_g_t_now, large_g_t_last, large_g_t_interpolated);
		sinoatrial_node_result[i][0] = t_now;
		sinoatrial_node_result[i][1] = x_now;
		sinoatrial_node_result[i][2] = y_now;

		/* rr間隔生成用 */
		if (0 < y_last && y_now <= 0) {
			t_at_y_equal_zero.push(t_now);
		}
		
		x_last = x_now;
		y_last = y_now;
		solve_differential_equation_serial_index++;
	}

	/* RR間隔を生成 */
	int rr_interval_result_size = t_at_y_equal_zero.size() - 1;
	rr_interval_result.assign(rr_interval_result_size, 0);
	for (int i = 0; i < rr_interval_result_size; i++) {
		double t_left = t_at_y_equal_zero.front(); t_at_y_equal_zero.pop();
		double t_right = t_at_y_equal_zero.front();
		rr_interval_result[i] = t_right - t_left;
	}
}
