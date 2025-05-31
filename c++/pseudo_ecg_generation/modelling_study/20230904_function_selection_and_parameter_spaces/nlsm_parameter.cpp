
#include "nlsm_parameter.h"


nlsm_parameter::nlsm_parameter(int distinct_point_num, int segment_step_num, double step_size, int function_parameter_num) {

 this->distinct_point_num = distinct_point_num;
 this->segment_step_num = segment_step_num;
 this->step_size = step_size;
 this->parameter_vector.resize(function_parameter_num);
 this->combination_vector.resize(distinct_point_num);

};