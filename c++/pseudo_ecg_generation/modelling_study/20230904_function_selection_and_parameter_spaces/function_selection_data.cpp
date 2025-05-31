
#include "function_selection_data.h"


function_selection_data::function_selection_data() {

 void_pointer_struct_pointer = new void_pointer_struct();

};


function_selection_data::~function_selection_data() {

 delete void_pointer_struct_pointer;

}


void function_selection_data::initialize_vector_size(int csv_file_num, int data_channel_num, int combination_num, int distinct_point_num, int max_paramter_num) {

 this->result_selected_combination_index_list.assign(csv_file_num, std::vector<int>(data_channel_num));

 this->result_selected_function_list.assign(csv_file_num, std::vector<std::vector<std::vector<int>>>(data_channel_num, std::vector<std::vector<int>>(combination_num, std::vector<int>(distinct_point_num))));
 
 this->result_selected_function_paramter_list.assign(csv_file_num, std::vector<std::vector<std::vector<double>>>(data_channel_num, std::vector<std::vector<double>>(combination_num, std::vector<double>(max_paramter_num))));
  
 this->result_selected_function_objective_function_list.assign(csv_file_num, std::vector<std::vector<double>>(data_channel_num, std::vector<double>(combination_num)));

 this->csv_file_record_num_vector.resize(csv_file_num);

 this->parameter_space.assign(data_channel_num, std::vector<std::vector<double>>(max_paramter_num, std::vector<double>(csv_file_num)));

}