

#include "csv.h"
#include "function_selection_data.h"
#include "process_for_one_combination.h"
#include "process_for_one_csv.h"
#include "process_for_post_optimization.h"
#include "combination.h"
//#include "C.h"


#include <iostream>
#include <vector>

#define INPUT_FOLDER_NAME "in_csv"
#define OUTPUT_FOLDER_NAME "out_csv"
#define OUTPUT_CSV_NAME_ADDITION_AND_EXTENSION "_optimized.csv"
#define ECG_CHANNEL_NUM 12
#define DISTINCT_POINT_NUM 5
#define STEP_SIZE 0.002
#define NLOPT_RELATIVE_TOLERANCE_ON_FUNCTION_VALUE 0.00001


// debug
std::vector<int> fitting_function_parameter_num_vector = { 3,3 };   /*  0 : 正規分布     3 */

//std::vector<int> fitting_function_parameter_num_vector = { 3,4 };   /*  0 : 正規分布     3 */
                                                                    /*  1 : 対数正規分布 4 */


int get_max_parameter_num(int scalar) {
 int tmp = 0;
 for (auto x : fitting_function_parameter_num_vector) {
  if (tmp < x) {
   tmp = x;
  }
 }
 return tmp * scalar;
}



int main()
{ 

 /* 本ファイルを含むプログラムファイルと同じ階層に存在するフォルダ名 */
 std::string input_folder_name = INPUT_FOLDER_NAME;

 /* 本ファイルを含むプログラムファイルと同じ階層に存在するフォルダ名  存在しなければ自動で作成する */
 std::string output_folder_name = OUTPUT_FOLDER_NAME;

 /* 出力する .csv名 は読み込む .csv名 にこの追加する文字列と拡張子をつけたもの */
 std::string output_csv_name_addition_and_extension = OUTPUT_CSV_NAME_ADDITION_AND_EXTENSION;

 /* .csv操作のクラスオブジェクト */
 csv csv_obj;

 /* .csvを出力するフォルダを作成 */
 csv_obj.create_output_folder(output_folder_name);

 /* フォルダ内を探索して全ての.csvファイル名をvector型のリストにする */
 std::vector<std::string> csv_file_name_vector = csv_obj.get_csv_file_name_vector(input_folder_name);

 /* .csv ファイルの数  */
 int csv_file_num = csv_file_name_vector.size();

 /* 12誘導 */
 int data_channel_num = ECG_CHANNEL_NUM;

 /* P Q R S T の 5つ */
 int distinct_point_num = DISTINCT_POINT_NUM;

 /* .csvのECGデータのステップサイズ(サンプリング) */
 double step_size = STEP_SIZE;


 /* 確率分布のパラメータの数の最大値 × 5 */

 int max_parameter_num = get_max_parameter_num(distinct_point_num);


 /* 相対許容誤差 */
 double nlopt_ftol = NLOPT_RELATIVE_TOLERANCE_ON_FUNCTION_VALUE;

 /*                                */
 /* 各クラスで共有するデータ構造体 */
 /*                                */
 //struct function_selection_data* function_selection_data_pointer = new function_selection_data(csv_file_num, data_channel_num, distinct_point_num, max_parameter_num);
 struct function_selection_data* function_selection_data_pointer = new function_selection_data();

 function_selection_data_pointer->input_folder_name = input_folder_name;
 function_selection_data_pointer->output_folder_name = output_folder_name;
 function_selection_data_pointer->csv_file_num = csv_file_num;
 function_selection_data_pointer->data_channel_num = data_channel_num;
 function_selection_data_pointer->distinct_point_num = distinct_point_num;
 function_selection_data_pointer->step_size = step_size;
 function_selection_data_pointer->max_parameter_num = max_parameter_num;
 function_selection_data_pointer->fitting_function_parameter_num_vector = fitting_function_parameter_num_vector;
 function_selection_data_pointer->csv_file_name_vector = csv_file_name_vector;
 function_selection_data_pointer->nlopt_ftol = nlopt_ftol;
 function_selection_data_pointer->output_csv_name_addition_and_extension = output_csv_name_addition_and_extension;



 /*                                              */
 /* フィッティングする関数の組合せのリストを作成 */
 /*                                              */
 int fitting_function_num = fitting_function_parameter_num_vector.size();
 combination combination_obj;
 combination_obj.generate_combination_matrix(function_selection_data_pointer->combination_matrix, function_selection_data_pointer->current_row, distinct_point_num, fitting_function_num, 0);
 function_selection_data_pointer->combination_num = function_selection_data_pointer->combination_matrix.size();

 std::cout << std::endl;
 std::cout << "---------------------------------------------------------------------------------------------------" << std::endl;
 std::cout << "フィッティングする関数組み合せのリストの数 : " << function_selection_data_pointer->combination_num << std::endl;
 std::cout << "---------------------------------------------------------------------------------------------------" << std::endl;
 std::cout << std::endl;


 /*                                                  */
 /* データ構造体のメンバ変数のvectorのサイズを初期化 */
 /*                                                  */
 int combination_num = function_selection_data_pointer->combination_num;
 function_selection_data_pointer->initialize_vector_size(csv_file_num, data_channel_num, combination_num, distinct_point_num, max_parameter_num);

 
 
 std::cout << "------------------ フィッティング処理 開始 --------------------------------------------------------" << std::endl;

 process_for_one_csv process_for_one_csv_obj(function_selection_data_pointer);
 process_for_one_combination process_for_one_combination_obj(function_selection_data_pointer);


 /* .csv ファイルの数だけ繰り返す  1被験者あたり1ファイルを想定  */
 for (int csv_cnt = 0; csv_cnt < csv_file_num; csv_cnt++) {

  std::cout <<  csv_cnt + 1 << " 番目のファイル : " << csv_file_name_vector[csv_cnt] << " を処理中 ... " << std::endl;

  /* .csv のインデックスを格納 */
  function_selection_data_pointer->void_pointer_struct_pointer->csv_cnt = csv_cnt;
  
  /*                            */
  /* 一つの .csv についての処理 */
  /*                            */
  process_for_one_csv_obj.process(csv_cnt);

  /* 誘導の数だけ繰り返す */
  for (int channel = 0; channel < data_channel_num; channel++) {
        
   /* フィッティング対象の誘導を意味するインデックスを格納 */
   function_selection_data_pointer->void_pointer_struct_pointer->channel_index = channel;

   /* Ⅲ, aVR, aVL, aVF は合成する誘導なのでフィッティングしない */
   if (2 <= channel && channel <= 5) {
	continue;
   }

   std::cout << " csv_cnt : " << csv_cnt << " channel : " << channel << std::endl;

   /*                                  */
   /* 関数の組合せリストについての処理 */
   /*                                  */
   process_for_one_combination_obj.process(csv_cnt, channel);

  }
  
 }
 /* .csv ファイルの数だけ繰り返す ここまで  */

 std::cout << "------------------ フィッティング処理 終了 --------------------------------------------------------" << std::endl;
 std::cout << std::endl;




 /* フィッティング後の処理のためのオブジェクト */
 process_for_post_optimization process_for_post_optimization_obj(function_selection_data_pointer);


 std::cout << "------------------ 最適化したパラメータで波形生成と.csv出力 開始 ----------------------------------" << std::endl;
 std::cout << std::endl;

 process_for_post_optimization_obj.generate_waveforms();

 std::cout << "------------------ 最適化したパラメータで波形生成と.csv出力 終了 ----------------------------------" << std::endl;
 std::cout << std::endl;




 std::cout << "------------------ 健康対照群のパラメータ空間生成 開始 --------------------------------------------" << std::endl;
 std::cout << std::endl;

 process_for_post_optimization_obj.generate_parameter_spaces();
  
 std::cout << "------------------ 健康対照群のパラメータ空間生成 終了 --------------------------------------------" << std::endl;
 std::cout << std::endl;



 delete function_selection_data_pointer;


 return 0;
}

