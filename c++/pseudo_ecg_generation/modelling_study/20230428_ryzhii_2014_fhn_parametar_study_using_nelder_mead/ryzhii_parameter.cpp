#include "ryzhii_parameter.h"



ryzhii_parameter::ryzhii_parameter() {

 this->step_size = 0.002;     /* [sec] */
 this->step_num = 5000;       /* シミュレーションステップ数  */

 this->vdp_variable_num = 3;
 this->a.resize(vdp_variable_num); a[0] = 40; a[1] = 50; a[2] = 50;
 this->f.resize(vdp_variable_num); f[0] = 22; f[1] = 8.4; f[2] = 1.5;
 this->e.resize(vdp_variable_num); e[0] = 3.5; e[1] = 5; e[2] = 12;
 this->u.resize(vdp_variable_num); u[0] = 0.6889; u[1] = 0.6889; u[2] = 0.6889;
 this->d.resize(vdp_variable_num); d[0] = 3; d[1] = 3; d[2] = 3;
 this->large_k.resize(vdp_variable_num); large_k[0] = 0; large_k[1] = 22; large_k[2] = 22;

 this->fhn_variable_num = 4;
 this->k.resize(fhn_variable_num); k[0] = 2e3; k[1] = 4e2; k[2] = 1e4; k[3] = 2e3;
 this->c.resize(fhn_variable_num); c[0] = 0.26; c[1] = 0.26; c[2] = 0.12; c[3] = 0.1;
 this->w_1.resize(fhn_variable_num); w_1[0] = 0.13; w_1[1] = 0.19; w_1[2] = 0.12; w_1[3] = 0.22;
 this->w_2.resize(fhn_variable_num); w_2[0] = 1.0; w_2[1] = 1.1; w_2[2] = 1.1; w_2[3] = 0.8;
 this->b.resize(fhn_variable_num); b[0] = 0; b[1] = 0; b[2] = 0.015;  b[3] = 0;
 this->h.resize(fhn_variable_num); h[0] = 0.004; h[1] = 0.004; h[2] = 0.008; h[3] = 0.008;
 this->g.resize(fhn_variable_num); g[0] = 0.4; g[1] = 0.4; g[2] = 0.09; g[3] = 0.1;   /* Ryzhii and Ryzhii(2014)では d  */
 this->large_c.resize(fhn_variable_num); large_c[0] = 4e-5; large_c[1] = 4e-5; large_c[2] = 9e-5; large_c[3] = 6e-5;

 this->x_initial_value = -0.1;
 this->y_initial_value = 0.025;
 this->z_initial_value = 0;
 this->v_initial_value = 0;

 this->delay_step_y = 46;      /* 時間遅れステップ数 論文では 0.092[s]  0.002 * 46 = 0.092 */
 this->ecg_num = 12;    /* 12誘導心電図 */

 /* Ryzhii and Ryzhii (2014) はⅡ誘導しか使わないので全部同じにしておく */
 this->alpha.resize(ecg_num, std::vector<double>(fhn_variable_num));
 alpha[0][0] = 1.0;   alpha[0][1] = 1.0; alpha[0][2] = 1.0;  alpha[0][3] = 1.0;
 alpha[1][0] = 1.0;   alpha[1][1] = 1.0;  alpha[1][2] = 1.0;  alpha[1][3] = 1.0;
 alpha[2][0] = 1.0;   alpha[2][1] = 1.0; alpha[2][2] = 1.0;   alpha[2][3] = 1.0;
 alpha[3][0] = 1.0;  alpha[3][1] = 1.0;   alpha[3][2] = 1.0; alpha[3][3] = 1.0;
 alpha[4][0] = 1.0;   alpha[4][1] = 1.0;  alpha[4][2] = 1.0;  alpha[4][3] = 1.0;
 alpha[5][0] = 1.0;  alpha[5][1] = 1.0;  alpha[5][2] = 1.0;  alpha[5][3] = 1.0;
 alpha[6][0] = 1.0;   alpha[6][1] = 1.0;   alpha[6][2] = 1.0; alpha[6][3] = 1.0;
 alpha[7][0] = 1.0;   alpha[7][1] = 1.0;    alpha[7][2] = 1.0; alpha[7][3] = 1.0;
 alpha[8][0] = 1.0;   alpha[8][1] = 1.0;   alpha[8][2] = 1.0;  alpha[8][3] = 1.0;
 alpha[9][0] = 1.0;    alpha[9][1] = 1.0;   alpha[9][2] = 1.0;  alpha[9][3] = 1.0;
 alpha[10][0] = 1.0;  alpha[10][1] = 1.0;  alpha[10][2] = 1.0; alpha[10][3] = 1.0;
 alpha[11][0] = 1.0;  alpha[11][1] = 1.0; alpha[11][2] = 1.0;  alpha[11][3] = 1.0;

 /* 出力用 */
 this->x_result.resize(step_num, std::vector<double>(vdp_variable_num));
 this->y_result.resize(step_num, std::vector<double>(vdp_variable_num));
 this->z_result.resize(step_num, std::vector<double>(fhn_variable_num));
 this->v_result.resize(step_num, std::vector<double>(fhn_variable_num));
 this->ecg_result.resize(step_num, std::vector<double>(ecg_num));

};