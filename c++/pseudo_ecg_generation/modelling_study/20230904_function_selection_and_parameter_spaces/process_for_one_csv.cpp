

#include "process_for_one_csv.h"


process_for_one_csv::process_for_one_csv(function_selection_data* function_selection_data_pointer) {

 this->function_selection_data_pointer = function_selection_data_pointer;

}


void process_for_one_csv::process(int csv_cnt) {


 int data_channel_num = function_selection_data_pointer->data_channel_num;
 int distinct_point_num = function_selection_data_pointer->distinct_point_num;

 csv csv_obj;

 /* .csv からフィッティング対象となるECG(実験データ) を読み込んで vector<double> に変換 : ecg_cut_out_data の サイズは 切り出しデータステップ数 × 12 */
 function_selection_data_pointer->ecg_cut_out_data = csv_obj.read_csv(function_selection_data_pointer->csv_file_name_vector[csv_cnt], function_selection_data_pointer->input_folder_name);

 int data_record_num = function_selection_data_pointer->ecg_cut_out_data.size(); function_selection_data_pointer->csv_file_record_num_vector[csv_cnt] = data_record_num;


 /* .csv ファイルごとに (data_record_num が異なるので) vector のサイズを初期化 */
 function_selection_data_pointer->void_pointer_struct_pointer->list_of_data_points.assign(data_record_num, std::vector<double>(data_channel_num));
 function_selection_data_pointer->void_pointer_struct_pointer->combination_vector.resize(distinct_point_num);

 function_selection_data_pointer->void_pointer_struct_pointer->csv_cnt = csv_cnt;
 function_selection_data_pointer->void_pointer_struct_pointer->step_size = function_selection_data_pointer->step_size;


 /* 各誘導について最初の値がゼロになるように縦方向にシフトして格納 */
 std::vector<double> initial_data_for_vertical_shift(data_channel_num);
 for (int channel = 0; channel < data_channel_num; channel++) {
  initial_data_for_vertical_shift[channel] = function_selection_data_pointer->ecg_cut_out_data[0][channel];
 }
 for (int channel = 0; channel < data_channel_num; channel++) {
  for (int i = 0; i < data_record_num; i++) {
   function_selection_data_pointer->void_pointer_struct_pointer->list_of_data_points[i][channel] = function_selection_data_pointer->ecg_cut_out_data[i][channel] - initial_data_for_vertical_shift[channel];
  }
 }

 function_selection_data_pointer->void_pointer = static_cast<void*>(function_selection_data_pointer->void_pointer_struct_pointer);

}

