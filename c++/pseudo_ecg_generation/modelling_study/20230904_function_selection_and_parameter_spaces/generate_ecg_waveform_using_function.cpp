
#include "generate_ecg_waveform_using_function.h"


#define PI 3.1415926535897932384626433832795028841971


/* モデルパラメータや初期値を意味するメンバ変数を初期化 */
generate_ecg_waveform_using_function::generate_ecg_waveform_using_function(struct nlsm_parameter* parameter_pointer) {

 this->distinct_point_num = parameter_pointer->distinct_point_num;
 this->step_size = parameter_pointer->step_size;
 this->segment_step_num = parameter_pointer->segment_step_num;
 this->parameter_vector = parameter_pointer->parameter_vector;
 this->combination_vector = parameter_pointer->combination_vector;
 this->parameter_index = 0;

}



double generate_ecg_waveform_using_function::normal_dist_function(double x) {
 double res;
 double scalar = parameter_vector[parameter_index];
 double m = parameter_vector[parameter_index + 1];
 double sd = parameter_vector[parameter_index + 2];

 // debug
 res = exp(-0.5 * (x - m) * (x - m) / (sd * sd));
 //res = exp(-0.5 * (x - m) * (x - m) / (sd * sd)) / fabs(sd);
 //res = exp(-0.5 * (x - m) * (x - m) / (sd * sd)) / (fabs(sd) * sqrt(2 * PI));
 res *= scalar;
 return res;
}

double generate_ecg_waveform_using_function::lognormal_dist_function(double x) {
 double res;
 double scalar = parameter_vector[parameter_index];
 double m = parameter_vector[parameter_index + 1];
 double sd = parameter_vector[parameter_index + 2];
 parameter_index++;   /* horizontal_shift 用 */
 res = exp(-0.5 * (log(x) - m) * (log(x) - m) / (sd * sd));
 res /= x;
 res *= scalar;
 return res;
}



void generate_ecg_waveform_using_function::generate_ecg_waveform_segment(std::vector<double>& ecg_segment_result) {


 /* P,Q,R,S,T の 5回 ここから */
 for (int distinct = 0; distinct < distinct_point_num; distinct++) {

  /* 使用する関数とそのパラメータ数 */
  int selected_function = this->combination_vector[distinct].first;
  int selected_function_parameter_num = this->combination_vector[distinct].second;


 /*                                                  */
 /*  関数を追加するときは ここの if文 に実装していく */
 /*                                                  */

 /*          */
 /* 正規分布 */
 /*          */
  if (selected_function == 0) {
   
   /* 時間方向 */
   for (int time_step = 0; time_step < segment_step_num; time_step++) {
    double x = time_step * step_size;
    ecg_segment_result[time_step] += normal_dist_function(x);
   } 

  }

  /*              */
  /* 対数正規分布 */
  /*              */
  else if (selected_function == 1) {

   /* 正規分布 */
   /* 時間方向 */
   for (int time_step = 0; time_step < segment_step_num; time_step++) {
    double x = time_step * step_size;
    ecg_segment_result[time_step] += normal_dist_function(x);
   }

 //  double horizontal_shift = parameter_vector[parameter_index + 3];
 //  int horizontal_shift_index = horizontal_shift / step_size;

 //  /* 時間方向 */
 //  for (int time_step = 0; time_step < segment_step_num; time_step++) {

 //   if (time_step < horizontal_shift_index) {
	// continue;
	//}

	//double x_shift = (time_step - horizontal_shift_index) * time_step;
	//ecg_segment_result[time_step] += lognormal_dist_function(x_shift);

 //  }

  }

  this->parameter_index += selected_function_parameter_num;



 }
 /* P,Q,R,S,T の 5回 ここまで */

}