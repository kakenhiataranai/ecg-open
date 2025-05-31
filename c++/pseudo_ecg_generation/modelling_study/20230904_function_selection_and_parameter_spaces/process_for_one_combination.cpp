

#include "process_for_one_combination.h"
#include "void_pointer_struct.h"


int iter_cnt;

/*                                                                  */
/* process_for_one_combinationクラス のメンバ関数ではないことに注意 */
/*                                                                  */
double objective_function(const std::vector<double>& parameter, std::vector<double>& parameter_grad, void* function_and_data) {


 //++iter_cnt;

 /* 使える型にしておく */
 void_pointer_struct* void_pointer_struct_pointer = static_cast<void_pointer_struct*> (function_and_data);

 int distinct_point_num = void_pointer_struct_pointer->combination_vector.size();
 int csv_cnt = void_pointer_struct_pointer->csv_cnt;
 int data_record_num = void_pointer_struct_pointer->list_of_data_points.size();
 double step_size = void_pointer_struct_pointer->step_size;
 int parameter_num = parameter.size();


 /*                                   */
 /* パラメータ構造体への入力ここから  */
 /*                                   */
 struct nlsm_parameter* parameter_pointer = new nlsm_parameter(distinct_point_num, data_record_num, step_size, parameter_num);

 parameter_pointer->parameter_vector = parameter;
 parameter_pointer->combination_vector = void_pointer_struct_pointer->combination_vector;


 /* ECG生成結果を格納する配列 */
 std::vector<double> ecg_segment_result(data_record_num);


 /* 心電図 ( = 確率密度関数の組合せ) の生成 計算実行 ここから */
 generate_ecg_waveform_using_function generate_obj(parameter_pointer);
 generate_obj.generate_ecg_waveform_segment(ecg_segment_result);
 /* ここまで */


 double squared_residual = 0;
 int time_step_num = void_pointer_struct_pointer->list_of_data_points.size();
 int channel = void_pointer_struct_pointer->channel_index;


 /* ノルムの二乗を計算 */
 for (int time_step = 0; time_step < time_step_num; time_step++) {
  double residual = void_pointer_struct_pointer->list_of_data_points[time_step][channel] - ecg_segment_result[time_step];
  squared_residual += residual * residual;
 }


 /* パラメータ構造体を破棄  */
 delete parameter_pointer;

 return squared_residual;
}


process_for_one_combination::process_for_one_combination(function_selection_data* function_selection_data_pointer) {

 this->function_selection_data_pointer = function_selection_data_pointer;

}


/*                                */
/* ある.csvのある誘導に対する処理 */
/*                                */
void process_for_one_combination::process(int csv_cnt, int channel) {

 int combination_num = function_selection_data_pointer->combination_num;
 int distinct_point_num = function_selection_data_pointer->distinct_point_num;

 double double_max = std::numeric_limits<double>::max();


  /* フィッティングする関数の組合せの数だけ繰り返す ここから */
 double current_min_value = double_max;
 for (int comb = 0; comb < combination_num; comb++) {

  int parameter_num = 0;

  /* 関数の組合せと対応するパラメータ数を準備 */
  for (int distinct = 0; distinct < distinct_point_num; distinct++) {

   /* 配列に入っている関数に割り当てられた値 */
   int tmp_function = function_selection_data_pointer->combination_matrix[comb][distinct];
   function_selection_data_pointer->void_pointer_struct_pointer->combination_vector[distinct].first = tmp_function;



   int tmp = function_selection_data_pointer->fitting_function_parameter_num_vector[tmp_function];
   parameter_num += tmp;
   function_selection_data_pointer->void_pointer_struct_pointer->combination_vector[distinct].second = tmp;

  }

  /* 最適化の初期値を設定 */
  std::vector<double> initial_value(parameter_num);
  for (int iv_idx = 0; iv_idx < parameter_num; iv_idx++) {

   initial_value[iv_idx] = 1;  // あとでかく .csv をロードする方向

  }

  double A_p = 0.03;
  double A_q = -0.05;
  double A_r = 0.88;
  double A_s = -0.12;
  double A_t = 0.07;


  /* P */
  initial_value[0] = A_p * 1.6768;
  initial_value[1] = 0.06;
  initial_value[2] = 0.04;
  
  /* Q */
  initial_value[3] = A_q * -0.0602;
  initial_value[4] = 0.2;
  initial_value[5] = 0.01;
  
  /* R */
  initial_value[6] = A_r * 0.9941;
  initial_value[7] = 0.23;
  initial_value[8] = 0.025;
  
  /* S */
  initial_value[9] = A_s * 0.9941;
  initial_value[10] = 0.27;
  initial_value[11] = 0.01;
  
  /* T */
  initial_value[12] = A_t * 1.199;
  initial_value[13] = 0.5;
  initial_value[14] = 0.1;


  std::vector<double> initial_value_opt = initial_value;


  /* NLopt の設定 */
  iter_cnt = 0;
  double min_value;
  nlopt::opt opt(nlopt::LN_NELDERMEAD, parameter_num);
  opt.set_min_objective(objective_function, function_selection_data_pointer->void_pointer);
  opt.set_ftol_rel(function_selection_data_pointer->nlopt_ftol);


  /* NLopt で最適化 */
  try {
   nlopt::result result = opt.optimize(initial_value_opt, min_value);
  }
  catch (std::exception& e) {
   std::cerr << "NLopt exception: " << e.what() << std::endl;
   exit(-1);
   //return 1;
  }

   
  /* 最新の最適化結果の方が目的関数値が小さいとき */
  /* 選択した組み合わせを置き換え                 */
  if (min_value < current_min_value) {
   function_selection_data_pointer->result_selected_combination_index_list[csv_cnt][channel] = comb;
   current_min_value = min_value;
  }

  /* 選択した関数を格納 */
  for (int distinct = 0; distinct < distinct_point_num; distinct++) {
   function_selection_data_pointer->result_selected_function_list[csv_cnt][channel][comb][distinct] = function_selection_data_pointer->combination_matrix[comb][distinct];
  }

  /* パラメータを格納 */
  for (int para = 0; para < parameter_num; para++) {
   function_selection_data_pointer->result_selected_function_paramter_list[csv_cnt][channel][comb][para] = initial_value_opt[para];
  }

  /* 最適化した目的関数の値を格納 */
  function_selection_data_pointer->result_selected_function_objective_function_list[csv_cnt][channel][comb] = min_value;

 }
 /* フィッティングする関数の組合せの数だけ繰り返す ここまで */

}

