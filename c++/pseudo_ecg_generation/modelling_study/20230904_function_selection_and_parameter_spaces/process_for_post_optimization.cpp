

#include "process_for_post_optimization.h"



process_for_post_optimization::process_for_post_optimization(function_selection_data* function_selection_data_pointer) {

 this->function_selection_data_pointer = function_selection_data_pointer;

}


void process_for_post_optimization::generate_waveforms() {

 int distinct_point_num = function_selection_data_pointer->distinct_point_num;
 int csv_file_num = function_selection_data_pointer->csv_file_num;
 int data_channel_num = function_selection_data_pointer->data_channel_num;
 double step_size = function_selection_data_pointer->step_size;

 csv csv_obj;

 /* 出力先のフォルダ名 */
 std::string output_folder_name = function_selection_data_pointer->output_folder_name;

 /* .csv ファイルの数だけ繰り返す */
 for (int csv_cnt = 0; csv_cnt < csv_file_num; csv_cnt++) {

  /* .csv のステップ数 */
  int data_record_num = function_selection_data_pointer->csv_file_record_num_vector[csv_cnt];

  /*                                   */
  /* .csv に出力する 12誘導 ECG の波形 */
  /*                                   */
  std::vector<std::vector<double>> optimized_ecg(data_record_num, std::vector<double>(data_channel_num));

  /* 誘導の数だけ繰り返す */
  for (int channel = 0; channel < data_channel_num; channel++) {

   if (2 <= channel && channel <= 5) {
	continue;
   }


   /* 計算済みの最適な組み合わせインデックス */
   int optimized_comb_index = function_selection_data_pointer->result_selected_combination_index_list[csv_cnt][channel];

   /* パラメータ数を作成 */
   int parameter_num_of_csv_now_and_channel_now = 0;
   for (int distinct = 0; distinct < distinct_point_num; distinct++) {
	int tmp_index = function_selection_data_pointer->result_selected_function_list[csv_cnt][channel][optimized_comb_index][distinct];
	parameter_num_of_csv_now_and_channel_now += function_selection_data_pointer->fitting_function_parameter_num_vector[tmp_index];
   }


   /*                                   */
   /* パラメータ構造体への入力ここから  */
   /*                                   */
   struct nlsm_parameter* parameter_pointer = new nlsm_parameter(distinct_point_num, data_record_num, step_size, parameter_num_of_csv_now_and_channel_now);

   
   /* 波形生成に用いるパラメータ */
   for (int para = 0; para < parameter_num_of_csv_now_and_channel_now; para++) {

	//parameter_pointer->parameter_vector[i] = function_selection_data_pointer->result_selected_function_paramter_list[csv_cnt][channel][i];
	parameter_pointer->parameter_vector[para] = function_selection_data_pointer->result_selected_function_paramter_list[csv_cnt][channel][optimized_comb_index][para];


	// debug
	std::cout << "parameter_pointer->parameter_vector[" << para << "] : " << parameter_pointer->parameter_vector[para] << std::endl;
	if (para % 3 == 2) {
	 std::cout << "------------------------------------------------------------------------------------" << std::endl;
	}

   }

   /* 波形生成に用いる関数 */
   for (int distinct = 0; distinct < distinct_point_num; distinct++) {
	parameter_pointer->combination_vector[distinct].first = function_selection_data_pointer->result_selected_function_list[csv_cnt][channel][optimized_comb_index][distinct];
	int tmp = parameter_pointer->combination_vector[distinct].first;
	parameter_pointer->combination_vector[distinct].second = function_selection_data_pointer->fitting_function_parameter_num_vector[tmp];

	//debug 
	std::cout << "parameter_pointer->combination_vector[distinct].first : " << parameter_pointer->combination_vector[distinct].first << std::endl;
   }


   /* ECG生成結果を格納する配列 */
   std::vector<double> ecg_segment_result(data_record_num);


   /* 心電図 ( = 確率密度関数の組合せ) の生成 計算実行 ここから */
   generate_ecg_waveform_using_function generate_obj(parameter_pointer);
   generate_obj.generate_ecg_waveform_segment(ecg_segment_result);
   /* ここまで */
   

   /* .csv出力するvectorに格納 */
   for (int i = 0; i < data_record_num; i++) {
	optimized_ecg[i][channel] = ecg_segment_result[i];
   }

  }
  /* 誘導の数だけ繰り返す ここまで */


  /* Ⅲ, aVR, aVL, aVF を合成から作成 */
  int lead_1_channel = 0;
  int lead_2_channel = 1;
  for (int channel = 0; channel < data_channel_num; channel++) {
   if (2 <= channel && channel <= 5) {
	for (int i = 0; i < data_record_num; i++) {
	 switch (channel) {
	 case 2:   /* Ⅲ */
	  optimized_ecg[i][channel] = optimized_ecg[i][lead_2_channel] - optimized_ecg[i][lead_1_channel];
	  break;
	 case 3:   /* aVR */
	  optimized_ecg[i][channel] = (-1) * (optimized_ecg[i][lead_1_channel] + optimized_ecg[i][lead_2_channel]) * 0.5;
	  break;
	 case 4:   /* aVL */
	  optimized_ecg[i][channel] = optimized_ecg[i][lead_1_channel] - optimized_ecg[i][lead_2_channel] * 0.5;
	  break;
	 case 5:   /* aVF */
	  optimized_ecg[i][channel] = optimized_ecg[i][lead_2_channel]- optimized_ecg[i][lead_1_channel] * 0.5;
	  break;
	 }
	}
   }
  }


  /*           */
  /* .csv 出力 */
  /*           */
  std::string s = function_selection_data_pointer->csv_file_name_vector[csv_cnt];
  std::string output_file_name = s.substr(0, s.find_last_of('.')); // 拡張子を取り除く
  std::string addition_and_extension = function_selection_data_pointer->output_csv_name_addition_and_extension;
  csv_obj.two_dimension_vector_double_to_csv(optimized_ecg, output_file_name + addition_and_extension, output_folder_name);
  
 }
 /* .csv ファイルの数だけ繰り返す ここまで */

}



void process_for_post_optimization::generate_parameter_spaces() {

 int data_channel_num = function_selection_data_pointer->data_channel_num;
 int max_paramter_num = function_selection_data_pointer->max_parameter_num;
 int csv_file_num = function_selection_data_pointer->csv_file_num;

 for (int channel = 0; channel < data_channel_num; channel++) {
  for (int para = 0; para < max_paramter_num; para++) {
   for (int csv_cnt = 0; csv_cnt < csv_file_num; csv_cnt++) {

	/* 計算済みの最適な組み合わせインデックス */
	int optimized_comb_index = function_selection_data_pointer->result_selected_combination_index_list[csv_cnt][channel];

	/*                      */
	/* パラメータ空間を作成 */
	/*                      */
	function_selection_data_pointer->parameter_space[channel][para][csv_cnt] = function_selection_data_pointer->result_selected_function_paramter_list[csv_cnt][channel][optimized_comb_index][para];

   }
  }
 }

 std::cout << "成功" << std::endl;

}