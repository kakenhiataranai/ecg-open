
#include "ecg_BVAM_model.h"



bvam_model::bvam_model(double large_c, double large_h, double beta, double gamma_t) {  /* モデルパラメータを意味するメンバ変数を初期化 */
	this->large_c = large_c;
	this->large_h = large_h;
	this->beta = beta;
	this->gamma_t = gamma_t;
}

std::vector<double> bvam_model::add_vector(std::vector<double> x_1, std::vector<double> x_2) {
	int num = x_1.size();
	std::vector<double> res(num, 0);
	for (int i = 0; i < num; i++) {
		res[i] = x_1[i] + x_2[i];
	}
	return res;
}

std::vector<double> bvam_model::constant_product_vector(double scalar, std::vector<double> x) {
	int num = x.size();
	std::vector<double> res(num, 0);
	res = x;
	for (int i = 0; i < num; i++) {
		res[i] *= scalar;
	}
	return res;
}

std::vector<double> bvam_model::derivative_x(std::vector<double> x) {
	int num = x.size();
	std::vector<double> res(num, 0);
	res[0] = x[0] - x[1] - large_c * x[0] * x[1] - x[0] * x[1] * x[1];
	res[1] = large_h * x[0] - 3 * x[1] + large_c * x[0] * x[1] + x[0] * x[1] * x[1] + beta * (x[3] - x[1]);
	res[2] = x[2] - x[3] - large_c * x[2] * x[3] - x[2] * x[3] * x[3];
	res[3] = large_h * x[2] - 3 * x[3] + large_c * x[2] * x[3] + x[2] * x[3] * x[3] + 2 * beta * (x[1] - x[3]);
	for (int i = 0; i < num; i++) {
		res[i] *= gamma_t;
	}
	return res;
}

void bvam_model::bvam_model_one_step_runge_kutta(std::vector<double>& x, std::vector<double> x_last, double step_size) {

	std::vector<double> k_1(variable_num);
	std::vector<double> k_2(variable_num);
	std::vector<double> k_3(variable_num);
	std::vector<double> k_4(variable_num);

	/* ルンゲクッタ開始 */
	k_1 = derivative_x(x_last);
	k_2 = derivative_x(add_vector(x_last, constant_product_vector(step_size * 0.5, k_1)));
	k_3 = derivative_x(add_vector(x_last, constant_product_vector(step_size * 0.5, k_2)));
	k_4 = derivative_x(add_vector(x_last, constant_product_vector(step_size, k_3)));
	
	std::vector<double> k_1_2(variable_num);
	std::vector<double> k_3_4(variable_num);	
	k_1_2 = add_vector(k_1, constant_product_vector(2, k_2));
	k_3_4 = add_vector(k_4, constant_product_vector(2, k_3));

	double scalar = step_size; scalar /= 6;
	x = add_vector(x_last, constant_product_vector(scalar, add_vector(k_1_2, k_3_4)));
}

/* モデルパラメータや初期値を意味するメンバ変数を初期化 */
ecg_signals::ecg_signals(double ecg_step_size, int ecg_one_cycle_step_num, double large_c, double large_h, double beta, double gamma_t, std::vector<double> alpha) : bvam_obj(large_c, large_h, beta, gamma_t) {

	step_size = ecg_step_size;
	one_cycle_step_num = ecg_one_cycle_step_num;

	x_initial.assign(x_num, 0);   /* 論文に書いてある初期値をセット */
	x_initial[2] = 0.1;           /* 論文に書いてある初期値をセット */
	x_last = x_initial;
	x_now.resize(x_num);
	alpha.resize(alpha_num);
	this->alpha = alpha;
}


void ecg_signals::generate_ecg_signal_one_cycle(std::vector<double>& ecg_result, std::vector<std::vector<double>>& x_result) {

	for (int i = 0; i < one_cycle_step_num; i++) {

		/* 微分方程式を1ステップ計算実行 Runge–Kutta */
		this->bvam_obj.bvam_model_one_step_runge_kutta(x_now, x_last, step_size);

		for (int j = 0; j < alpha_num; j++) {
			ecg_result[i] += alpha[j] * x_now[j];
		}
		for (int j = 0; j < x_num; j++) {
			x_result[i][j] = x_now[j];
		}
		x_last = x_now;
	}
}