/* ecg_model_parameter.h */

#ifndef ECG_MODEL_PARAMETER_H
#define ECG_MODEL_PARAMETER_H

#include <vector>


/* ���f���p�����[�^�p�\����  */
struct ecg_model_parameter {

 ecg_model_parameter();

public:

 int ecg_num;                       /* 12�U��                */
 int mcsharry_variable_num;         /* x, y, z, omega �� 4�� */
 int variable_num;                  /* x, y, z �� 3��        */
 int distinct_point_num;            /* P, Q, R, S, T �� 5��  */

 double step_size;                  /* [sec] */
 int step_num_per_sec;              /* = 1 / 0.002  */
 int step_num;                      /* �V�~�����[�V�����X�e�b�v��  */

 std::vector<std::vector<double>> theta;  /* channel_num �~ distinct_point_num */
 std::vector<std::vector<double>> a;      /* channel_num �~ distinct_point_num */
 std::vector<std::vector<double>> b;      /* channel_num �~ distinct_point_num */
 std::vector<std::vector<double>> alpha;  /* channel_num �~ distinct_point_num */


 double x_initial;
 double y_initial;
 double z_initial;
 double omega_initial;
 int omega_t_size;

 double z_scalar;

 double large_a;
 double f_2;

};





#endif
