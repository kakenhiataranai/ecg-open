
#include "fasade_generate_12_lead_ecg.h"



fasade_child_process_ecg::fasade_child_process_ecg(struct ecg_model_parameter* ecg_model_parameter_pointer)
                                
                                 : ecg_signals_obj(ecg_model_parameter_pointer) {}


void fasade_child_process_ecg::fasade_child_process_ecg_execute(struct shared_memory* shared_memory_pointer) {
  
 int ecg_num = fasade_child_process_ecg::ecg_signals_obj.get_ecg_num();
 int mcsharry_variable_num = fasade_child_process_ecg::ecg_signals_obj.get_mcsharry_variable_num();
 int one_cycle_step_num = fasade_child_process_ecg::ecg_signals_obj.get_one_cycle_step_num();

 /* 結果を格納する配列 サイズは ecg_num(=12) × ( one_cycle_step_num(=5) × mcsharry_variable_num(=4) ) */;
 std::vector<std::vector<std::vector<double>>> result(ecg_num, std::vector<std::vector<double>>(one_cycle_step_num, std::vector<double>(mcsharry_variable_num)));

 this->ecg_signals_obj.generate_12_lead_ecg_signals_one_cycle(result);


 /* 共有メモリに書き込む  */
 const int z_index = 3;
 for (int channel= 0; channel < ecg_num; channel++) {
  for (int step = 0; step < one_cycle_step_num; step++) {
   shared_memory_pointer->child_process_ecg_array[step][channel] = result[channel][step][z_index];
  }
 }

}

