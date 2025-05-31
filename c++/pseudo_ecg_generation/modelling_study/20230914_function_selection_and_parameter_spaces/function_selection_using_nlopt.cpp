#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <vector>
#include <cmath>
#include <limits>
#include <nlopt.hpp>


/////////////////// CLASS外関数

// 円周率 for 密度関数
template<typename T> T pi(){
 return T(4)*atan(T(1));
}
#define PI pi<double>()

/*  vector<double> の和 を返す関数  */
std::vector<double> add_vector_double(std::vector<double> &x, std::vector<double> &y) {
 int res_size = x.size();
 std::vector<double> res(res_size);
 for (int i = 0; i < res_size; i++) {
  res[i] = x[i] + y[i];
 }
 return res;
}

/*  vector<double> の差 を返す関数 x-y */
std::vector<double> sub_vector_double(std::vector<double> &x, std::vector<double> &y) {
 int res_size = x.size();
 std::vector<double> res(res_size);
 for (int i = 0; i < res_size; i++) {
  res[i] = x[i] - y[i];
 }
 return res;
}

/* vector<double> の最小値とそのインデックスを返す関数 */
std::pair<double, size_t> find_min_value_and_index(const std::vector<double>& v){
 double minValue = std::numeric_limits<double>::max();;
 size_t minIndex = 0;
 for (size_t i = 0; i < v.size(); ++i) {
  if (v[i] < minValue) {
   minValue = v[i];
   minIndex = i;
  }
 }
 return std::make_pair(minValue, minIndex);
}

/* vector<double> の最大値とそのインデックスを返す関数 */
std::pair<double, size_t> find_max_value_and_index(const std::vector<double>& v){
 double maxValue = std::numeric_limits<double>::min();;
 size_t maxIndex = 0;
 for (size_t i = 0; i < v.size(); ++i) {
  if (v[i] > maxValue){
   maxValue = v[i];
   maxIndex = i;
  }
 }
 return std::make_pair(maxValue, maxIndex);
}

/* 対数正規分布の最頻値を計算する関数 */
double lognormal_dist_mode_value(double mu,double sigma, double shift){
 return shift + exp(mu - sigma * sigma);
}

/*  フィッティングする関数のパラメータ数の総和 を計算する関数  */
int get_max_parameter_num(std::vector<int> 
 combination_vector, std::vector<int> fitting_function_parameter_num_vector) {
 int res = 0;
 for (int i = 0; i < combination_vector.size(); i++) {
  res += fitting_function_parameter_num_vector[combination_vector[i]];
 }
 return res;
}


/////////////////// CLASS定義色々

//////// Csvクラス

class Csv{
 public:
 /* フォルダの存在を確認するための関数 */
 bool is_exist_folder(const std::string& folder_name) {
  if (std::filesystem::exists(folder_name)) {
   return true;
  }
  else {return false;}
 }

 /* ファイル名を指定して2次元csvを読み込む */
 template<typename T>
 std::vector<std::vector<T>> read_csv(const std::string& file_name){
  std::vector<std::vector<T>> data;
  std::ifstream file(file_name);
  if (!file.is_open()) {
   std::cerr << "Failed to open file: " << file_name << std::endl;
   exit(-1);
  }
  std::string line;
  while (std::getline(file, line)) {
   std::vector<T> row;
   std::istringstream iss(line);
   std::string value;
   while (std::getline(iss, value, ',')) {
    try {
     double number = std::stod(value);
     row.push_back(number);
    }
    catch (const std::exception& e) {
     std::cerr << "Error converting value to double: " << value << std::endl;
    }
   }
   data.push_back(row);
  }
  file.close();
  return data;
 }

 /* 二次元の vector を .csv に出力するための関数 */
 template<typename T>
 void two_dimension_vector_to_csv(
  std::vector<std::vector<T>> 
   vector_name, const std::string& file_name, const std::string& folder_name
 ){
  int row_num = vector_name.size();
  int column_num = vector_name.at(0).size();

  /* Open a file stream for writing */
  std::ofstream file(folder_name + "/" + file_name);

  /*  Write the data to the file */
  for (int i = 0; i < row_num; i++) {
   for (int j = 0; j < column_num; j++) {
   /* Write the element to the file */
    file << vector_name[i][j];
   /* Add a comma after the element, unless it's the last element in the row */
    if (j < column_num - 1) {
    file << ",";
   }
  }
  /* Add a new line after each row */
  file << "\n";
 }
 file.close();
 }
};//Csvクラス終わり




//////// Void_pointerクラス
//objective_functionに渡すためのもの

class Void_pointer {
 public:
 /* .csv からフィッティング対象となるECG(実験データ) を読み込んで
     二次元の vector<double> に変換したもの */
 std::vector<std::vector<double>> list_of_data_points;  

 /*  サイズは 切り出しデータステップ数 × 12 */
 /*  first  : フィッティングする関数を意味するインデックス 要素数 5 (P,Q,R,S,T) */
 /*  second : first の関数のパラメータ数                                        */
 std::vector<std::pair<int, int>> combination_vector;
 int channel_index;
 double step_size;
 std::vector<std::pair<double, size_t>> min_value_and_index_vector;
 std::vector<std::pair<double, size_t>> max_value_and_index_vector;
 int constraint_index;

};//Void_pointerクラス終わり



//////// Dataクラス
//プログラムの多くのクラスで用いる共用のデータ構造体
class Data{

 public:
 Data() : void_pointer(){};
 std::string csv_file_name;
 std::string output_folder_name;
 std::vector<int> fitting_function_parameter_num_vector;

 int data_channel_num;
 int distinct_point_num;
 double step_size;
 int max_parameter_num;
 std::vector<std::vector<int>> combination_matrix;
 std::vector<std::vector<double>> ecg_cut_out_data;

 Void_pointer void_pointer;
 void* void_pointer_real;

 std::vector<std::vector<double>> nlopt_initial_value_matrix;
 double nlopt_ftol;



 /* 各.csv の各誘導 について最も最適となる(目的関数値が小さくなる)組み合わせインデックス */
 /* サイズは  data_channel_num(=12) */
 std::vector<int> result_selected_combination_index_list;

 /* 入力した関数組み合わせに対応するパラメータの値       */
 /* サイズは  data_channel_num(=12) × max_paramter_num  */
 std::vector<std::vector<double>> result_paramter_list;


 /* 入力した関数組み合わせに対応する目的関数の値  */
 /* サイズは  data_channel_num(=12)               */
 std::vector<double> result_selected_function_objective_function_list;


 /* パラメータ空間 */
 /* サイズは data_channel_num(=12) × max_paramter_num × .csvのファイル数 */
 std::vector<std::vector<std::vector<double>>> parameter_space;

 std::string output_csv_name_addition_and_extension_0;
 std::string output_csv_name_addition_and_extension_1;



 void initialize_vector_size(
  int data_channel_num, int distinct_point_num, int max_paramter_num
 ) {

  this->result_selected_combination_index_list.resize(data_channel_num);

  this->result_paramter_list.assign(data_channel_num,
   (std::vector<double>(max_paramter_num))
  );

  this->result_selected_function_objective_function_list.resize(data_channel_num);

  this->parameter_space.assign(data_channel_num, std::vector<std::vector<double>>(
   max_paramter_num, std::vector<double>(1))
  );
 }

};// Dataクラス終わり





//////// Nlsmクラス
//非線形最小二乗法 nlsm (nonlinear least squares method) 用データ
class Nlsm{
 public:
 Nlsm(int distinct_point_num, int segment_step_num, 
  double step_size, int function_parameter_num) {
  this->distinct_point_num = distinct_point_num;
  this->segment_step_num = segment_step_num;
  this->step_size = step_size;
  this->parameter_vector.resize(function_parameter_num);
  this->combination_vector.resize(distinct_point_num);
 }

 int distinct_point_num;            /* P, Q, R, S, T の 5つ        */
 int segment_step_num;              /* 切り出し波形のステップ数    */
 double step_size;                  /* ステップサイズ (0.002[sec]) */

 /* 関数のパラメータ列　function_parameter_num */
 std::vector<double> parameter_vector;

 /* first  : フィッティングする関数を意味するインデックス 要素数 5 (P,Q,R,S,T) */
 /* second : first で指定した関数のパラメータ数                                */
 std::vector<std::pair<int, int>> combination_vector;
};//////// Nlsmクラス終わり




//////// Generate_waveformクラス
//ECGの波形を生成するためのクラス
//フィッテイングに用いる関数はこのクラスに実装
class Generate_waveform{

 public:
 /* モデルパラメータや初期値を意味するメンバ変数を初期化 */
 Generate_waveform(Nlsm* nlsm_p) {
  this->distinct_point_num = nlsm_p->distinct_point_num;
  this->step_size = nlsm_p->step_size;
  this->segment_step_num = nlsm_p->segment_step_num;
  this->parameter_vector = nlsm_p->parameter_vector;
  this->combination_vector = nlsm_p->combination_vector;
  this->parameter_index = 0;
 }

 void generate_segment(std::vector<double>& ecg_segment_result) {
  /* P,Q,R,S,T の 5回 ここから */
  for (int distinct = 0; distinct < distinct_point_num; distinct++) {
   /* 使用する関数とそのパラメータ数 */
   int selected_function = this->combination_vector[distinct].first;
   int selected_function_parameter_num = this->combination_vector[distinct].second;

   /*  関数を追加するときは ここの if文 に実装していく */


   /* 正規分布 */
   if (selected_function == 0) {
   /* 時間方向 */
    for (int time_step = 0; time_step < segment_step_num; time_step++) {
     double x = time_step * step_size;
     ecg_segment_result[time_step] += normal_dist_function(x);
     } 
    }

   /* 対数正規分布 */
   else if (selected_function == 1) {
    double horizontal_shift = parameter_vector[parameter_index + 3];
    /* 時間方向 */
    for (int time_step = 0; time_step < segment_step_num; time_step++) {
     if (time_step * step_size <= horizontal_shift) {
      continue;
     }
     double x_shift = time_step * step_size - horizontal_shift;
     ecg_segment_result[time_step] += lognormal_dist_function(x_shift);
    }
   }
   this->parameter_index += selected_function_parameter_num;
  }
 }

 private:
 int distinct_point_num;  /* P, Q, R, S, T の 5つ       */
 int segment_step_num;    /* 切り出し波形のステップ数   */
 double step_size;
 int parameter_index;
 std::vector<double> parameter_vector;                  /* 関数のパラメータ列 */

 /*  first  : フィッティングする関数を意味するインデックス 要素数 5 (P,Q,R,S,T) */
 /*  second : first の関数のパラメータ数                                        */
 std::vector<std::pair<int, int>> combination_vector; 

 double normal_dist_function(double x) {
  double res;
  double scalar = parameter_vector[parameter_index];
  double m = parameter_vector[parameter_index + 1];
  double sd = parameter_vector[parameter_index + 2];

  // sekiguchi
  double y_shift = parameter_vector[parameter_index + 3];

  //res = exp(-0.5 * (x - m) * (x - m) / (sd * sd)) / (sd * sqrt(2. * PI));
  res = exp(-0.5 * (x - m) * (x - m) / (sd * sd));
  res *= scalar;

  // sekiguchi
  res += y_shift;

  return res;
 }

 double lognormal_dist_function(double x) {
  double res;
  double scalar = parameter_vector[parameter_index];
  double mu = parameter_vector[parameter_index + 1];
  double sigma = parameter_vector[parameter_index + 2];
  double abs_x = fabs(x);
  res = exp(-0.5 * (log(abs_x) - mu) * (log(abs_x) - mu) / (sigma * sigma));
  res /= (sqrt(2. * PI) * sigma * abs_x);
  res *= scalar;
  return res;
 }
};





//////// Process_for_one_csvクラス
//.csv ファイル１つずつに対する処理
class Process_for_one_csv {
 public:
 Process_for_one_csv(Data* data_p) { 
  this->data_p = data_p;
 }
 void process(void) {
  int data_channel_num = data_p->data_channel_num;
  int distinct_point_num = data_p->distinct_point_num;
  double step_size = data_p->step_size;
  Csv csv_obj;

  /* .csv からフィッティング対象となるECG(実験データ) を読み込んで 
      vector<double> に変換 : 
      ecg_cut_out_data の サイズは 切り出しデータステップ数 × 12 */
  data_p->ecg_cut_out_data
   =csv_obj.read_csv<double>(data_p->csv_file_name);

  int data_record_num = data_p->ecg_cut_out_data.size(); 

  /* .csv ファイルごとに (data_record_num が異なるので) vector のサイズを初期化 */
  data_p->void_pointer.list_of_data_points.assign(
   data_record_num, std::vector<double>(data_channel_num)
  );
  data_p->void_pointer.combination_vector.resize(distinct_point_num);
  data_p->void_pointer.step_size 
   = data_p->step_size;


  /* 各誘導について最初の値がゼロになるように縦方向にシフトして格納 */
  for (int channel = 0; channel < data_channel_num; channel++) {
   double initial_data = data_p->ecg_cut_out_data[0][channel];
   for (int i = 0; i < data_record_num; i++) {
    double tmp = data_p->ecg_cut_out_data[i][channel];
    data_p->void_pointer.list_of_data_points[i][channel] = tmp - initial_data;
   }
  }

  /* 各誘導について最後の値がゼロになるように縦方向にシフトして格納 */
  for (int channel = 0; channel < data_channel_num; channel++) {
   double last_data = data_p->void_pointer.list_of_data_points[data_record_num - 1][channel];
   for (int i = 0; i < data_record_num; i++) {
    double tmp = data_p->void_pointer.list_of_data_points[i][channel];
    data_p->void_pointer.list_of_data_points[i][channel]
     = tmp - last_data * (double)i / (double)data_record_num;   
   }
  }

  /* 各誘導について最大値と最小値とそれらのときのインデックスを取得 */
  std::vector<std::pair<double, size_t>> min_value_and_index_vector(data_channel_num);
  std::vector<std::pair<double, size_t>> max_value_and_index_vector(data_channel_num);
  for (int channel = 0; channel < data_channel_num; channel++) {
   std::vector<double> tmp(data_record_num);
   for (int i = 0; i < data_record_num; i++) {
    tmp[i] = data_p->void_pointer.list_of_data_points[i][channel];
   }
   min_value_and_index_vector[channel] = find_min_value_and_index(tmp);
   max_value_and_index_vector[channel] = find_max_value_and_index(tmp);
  }
  data_p->void_pointer.min_value_and_index_vector = min_value_and_index_vector;
  data_p->void_pointer.max_value_and_index_vector = max_value_and_index_vector;

  /* NLoptの引数に適合する型に変換 */
  data_p->void_pointer_real = static_cast<void*>(
   &data_p->void_pointer
   );


  // debug
  csv_obj.two_dimension_vector_to_csv(data_p->void_pointer.list_of_data_points, "data.csv", "out");

 }
 private:
 Data* data_p;
};// Process_for_one_csvクラスここまで





/* Process_for_one_combinationクラス のメンバ関数ではないことに注意 */
double objective_function(
 const std::vector<double>& parameter, 
 std::vector<double>& parameter_grad, 
 void* function_and_data
 ){

 /* 使える型にしておく */
 Void_pointer* void_pointer = static_cast<Void_pointer*> (function_and_data);

 int distinct_point_num = void_pointer->combination_vector.size();
 int data_record_num = void_pointer->list_of_data_points.size();
 double step_size = void_pointer->step_size;
 int parameter_num = parameter.size();


 /* パラメータ構造体への入力ここから  */
 Nlsm* parameter_pointer 
  = new Nlsm(distinct_point_num, data_record_num, step_size, parameter_num);

 parameter_pointer->parameter_vector = parameter;
 parameter_pointer->combination_vector = void_pointer->combination_vector;


 /* ECG生成結果を格納する配列 */
 std::vector<double> ecg_segment_result(data_record_num);


 /* 心電図 ( = 確率密度関数の組合せ) の生成 計算実行 ここから */
 Generate_waveform generate_obj(parameter_pointer);
 generate_obj.generate_segment(ecg_segment_result);
 /* ここまで */


 double squared_residual = 0.;
 int time_step_num = void_pointer->list_of_data_points.size();
 int channel = void_pointer->channel_index;

 /* ノルムの二乗和を計算 */
 for (int time_step = 0; time_step < time_step_num; time_step++) {
  double residual 
   // sekiguchi
   = void_pointer->list_of_data_points[time_step][channel] - ecg_segment_result[time_step];
   // = void_pointer->list_of_data_points[time_step][channel] - ecg_segment_result[time_step] - 0.1 * parameter[parameter_num - 1];
  squared_residual += (residual * residual);
 }

 /* パラメータ構造体を破棄  */
 delete parameter_pointer;
 return squared_residual;
}

double myvconstraint(
 const std::vector<double>& parameter, 
 std::vector<double>& parameter_grad, 
 void* function_and_data) {

 /* 使える型にしておく */
 Void_pointer* void_pointer = static_cast<Void_pointer*> (function_and_data);

 /* 不等式制約条件の仮番号 */
 int constraint_index = void_pointer->constraint_index;

 int channel = void_pointer->channel_index;
 double step_size = void_pointer->step_size;


 /* 制約の計算 */
 /* 不等式制約の向きを表す符号 */
 int t = 1;

 double mu;
 double sigma;
 double d;

 /* R波の等式制約 */
 if (constraint_index == 0 || constraint_index == 1) {
  if (constraint_index == 1) {
   t = -1;
  }
  mu = parameter[5];
  sigma = parameter[6];
  d = parameter[7];
 }
 /* Q波 or S波 の不等式制約 */
 else if (constraint_index == 2) {

  size_t min_index = void_pointer->min_value_and_index_vector[channel].second;
  size_t max_index = void_pointer->max_value_and_index_vector[channel].second;

  if (min_index < max_index) {
   /* Q波の不等式制約  */
   mu = parameter[1];
   sigma = parameter[2];
   d = parameter[3];

  }
  else {
   /* S波の不等式制約 */
   t = -1;
   mu = parameter[9];
   sigma = parameter[10];
   d = parameter[11];
  }
  
 }

 double time_at_r_peak 
  = void_pointer->max_value_and_index_vector[channel].second * step_size;

 return t * (lognormal_dist_mode_value(mu, sigma, d) - time_at_r_peak);
}




//誘導１つについての処理クラス
class Process_for_one_combination {
 public:
 Process_for_one_combination(Data* data_p) { 
  this->data_p = data_p; 
 }

 /* ある誘導に対する処理 */
 void process(int channel) {
  
  int distinct_point_num = data_p->distinct_point_num;
  int parameter_num = 0;
  
  for (int distinct = 0; distinct < distinct_point_num; distinct++) {

   /* 配列に入っている関数に割り当てられた値 */
   int tmp_function = data_p->combination_matrix[0][distinct];
   data_p->void_pointer.combination_vector[distinct].first 
    = tmp_function;

   int tmp = data_p->fitting_function_parameter_num_vector[tmp_function];
   parameter_num += tmp;
   data_p->void_pointer.combination_vector[distinct].second 
    = tmp;
  }

  /* 最適化の初期値を設定 */
  std::vector<double> initial_value(parameter_num);
  for (int iv_idx = 0; iv_idx < parameter_num; iv_idx++) {
   initial_value[iv_idx] = data_p->nlopt_initial_value_matrix[iv_idx][channel];
  }
  std::vector<double> initial_value_opt = initial_value;

  /* NLopt の設定 */
  double min_value;
  nlopt::opt opt(nlopt::LN_NELDERMEAD, parameter_num);
  //nlopt::opt opt(nlopt::LN_COBYLA, parameter_num);
  //nlopt::opt opt(nlopt::LN_BOBYQA, parameter_num);

  std::vector<double> lb(parameter_num);
  //std::vector<double> ub(parameter_num);


  /* sigma は正の値になる制約 */
  /* param は scalar,mu,sigma, y_shift の順にならんでいるので 4で割って余り2がsigmaに対応 */
  /* !!!! */
  for (int i = 0; i < parameter_num; i++) {
   //if (i % 3 == 2) {

   // sekiguchi
   if (i % 4 == 2) {

    lb[i] = 1e-12;
   }
   else {
    lb[i] = (-1) * HUGE_VAL;
   }
  }




  ///* sigma は正の値になる制約 */
  ///* param は scalar,mu,sigma の順にならんでいるので 3で割って余り2がsigmaに対応 */
  ///* !!!! */
  //for (int i = 0; i < parameter_num; i++) {
  // if (i % 3 == 2) {
  //  lb[i] = 1e-12;
  // }
  // else {
  //  lb[i] = (-1) * HUGE_VAL;
  // }
  //}


  ///* sigma は正の値になる制約 */
  ///* param は scalar,mu,sigma,d の順にならんでいるので 4で割って余り2がsigmaに対応 */
  ///* !!!! */
  //for (int i = 0; i < parameter_num; i++) {
  // if (i % 4 == 2) {
  //  lb[i] = 1e-12;
  // }
  // else {
  //  lb[i] = (-1) * HUGE_VAL;
  // }
  //}

  ///* Q,S の scalar は負の値になる制約 */
  ///* param は scalar,mu,sigma,d の順にならんでいるので 4で割って余り0がscalarに対応 */
  ///* ただしparam の 4 番目は R波 なので制約を付けない */
  ///* !!!! */
  //for (int i = 0; i < parameter_num; i++) {
  // if ((i % 4 == 0)&&(i != 4)) {
  //  ub[i] = (-1) * 1e-12;
  // }
  // else {
  //  ub[i] = HUGE_VAL;
  // }
  //}

  opt.set_lower_bounds(lb);
  //opt.set_upper_bounds(ub);

  
  opt.set_min_objective(objective_function, data_p->void_pointer_real); 

  /* 等式制約1つと不等式制約を1つ設定 */
  //for (int i = 0; i < 3; i++) {
  // data_p->void_pointer.constraint_index = i;
  // opt.add_inequality_constraint(myvconstraint, data_p->void_pointer, 1e-8);
  //}

  opt.set_ftol_rel(data_p->nlopt_ftol);

  /* NLopt で最適化 */
  try {
   nlopt::result result = opt.optimize(initial_value_opt, min_value);
  }
  //エラーの時はとめる
  catch (std::exception& e) {
   std::cerr << "NLopt exception: " << e.what() << std::endl;
   exit(-1);
  }

  /* パラメータを格納 */
  for (int para = 0; para < parameter_num; para++) {
   data_p->result_paramter_list[channel][para]
    = initial_value_opt[para];
  }

  /* 最適化した目的関数の値を格納 */
  data_p->result_selected_function_objective_function_list[channel] = min_value;
 }

 /* 初期値を準備するメソッドを書く予定 */
 private:
 Data* data_p;
};



//フィティング後の処理クラス
class Process_for_post_optimization {

 public:
 Process_for_post_optimization(Data* data_p) { 
  this->data_p = data_p; 
 }

 /* フィッティングにより得た関数とパラメータを用いてECG波形を生成するメソッド */
 void generate_waveforms() {
  int distinct_point_num = data_p->distinct_point_num;
  int data_channel_num = data_p->data_channel_num;
  int parameter_num = data_p->max_parameter_num;
  double step_size = data_p->step_size;

  Csv csv_obj;

  /* 出力先のフォルダ名 */
  std::string output_folder_name = data_p->output_folder_name;

  /* .csv のステップ数 */
  //int data_record_num = data_p->data_record_num;
  int data_record_num = data_p->ecg_cut_out_data.size();
 
  /* .csv に出力する 12誘導 ECG の波形 */
  std::vector<std::vector<double>> 
   optimized_ecg(data_record_num, std::vector<double>(data_channel_num));

  /* .csv に出力する 12誘導 それぞれの選択する関数 */
  std::vector<std::vector<int>> 
   result_selected_function(distinct_point_num, std::vector<int>(data_channel_num));

  /* .csv に出力する 12誘導 それぞれのパラメータ */
  std::vector<std::vector<double>> 
   optimized_parameter(parameter_num, std::vector<double>(data_channel_num));


  /* 誘導の数だけ繰り返す */
  for (int channel = 0; channel < data_channel_num; channel++) {
   if (2 <= channel && channel <= 5) {
    continue;
   }

      
   /* パラメータ数 */
   int parameter_num = data_p->max_parameter_num;
   

   /* パラメータ構造体  */
   Nlsm* parameter_pointer 
    = new Nlsm(distinct_point_num, data_record_num, 
               step_size, parameter_num);

   
   /* 波形生成に用いるパラメータ */
   for (int para = 0; para < parameter_num; para++) {

    parameter_pointer->parameter_vector[para] 
     = data_p->result_paramter_list[channel][para];
    optimized_parameter[para][channel] 
     = data_p->result_paramter_list[channel][para];

    // debug
    if (channel == 1) {
     std::cout 
      << "parameter_pointer->parameter_vector[" << para << "] : " 
      << parameter_pointer->parameter_vector[para] << std::endl;
     if (para % 4 == 3) {
      std::cout << "---" << std::endl;
     }
    }
   }

   /* 波形生成に用いる関数 */
   //for (int distinct = 0; distinct < distinct_point_num; distinct++) {
   // parameter_pointer->combination_vector[distinct].first 
   //  = data_p->result_selected_function_list[channel][optimized_comb_index][distinct];
   // result_selected_function[distinct][channel] 
   //  = parameter_pointer->combination_vector[distinct].first;
   // int tmp = parameter_pointer->combination_vector[distinct].first;
   // parameter_pointer->combination_vector[distinct].second 
   //  = data_p->fitting_function_parameter_num_vector[tmp];
   // }


   /* ECG生成結果を格納する配列 */
   std::vector<double> ecg_segment_result(data_record_num);


   /* 心電図 ( = 確率密度関数の組合せ) の生成 計算実行 ここから */
   Generate_waveform generate_obj(parameter_pointer);
   generate_obj.generate_segment(ecg_segment_result);
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
      optimized_ecg[i][channel] 
       = optimized_ecg[i][lead_2_channel] - optimized_ecg[i][lead_1_channel];
      break;
      case 3:   /* aVR */
      optimized_ecg[i][channel] 
       = (-1) * (optimized_ecg[i][lead_1_channel] + optimized_ecg[i][lead_2_channel])*.5;
      break;
      case 4:   /* aVL */
      optimized_ecg[i][channel] 
       = optimized_ecg[i][lead_1_channel] - optimized_ecg[i][lead_2_channel] * 0.5;
      break;
      case 5:   /* aVF */
      optimized_ecg[i][channel] 
       = optimized_ecg[i][lead_2_channel] - optimized_ecg[i][lead_1_channel]*.5;
      break;
     }
    }
   }
  }

  /* .csv 出力 */
  std::string s = data_p->csv_file_name; //_vector[csv_cnt];
  std::string output_file_name = s.substr(0, s.find_last_of('.')); // 拡張子を取り除く
  int found_idx = output_file_name.find('/'); // 文字列のなかの / のインデックスを取得
  output_file_name = output_file_name.substr(found_idx + 1);
  std::string addition_and_extension_0 = data_p->output_csv_name_addition_and_extension_0;
  std::string addition_and_extension_1 = data_p->output_csv_name_addition_and_extension_1;

  csv_obj.two_dimension_vector_to_csv(
   optimized_ecg, output_file_name + addition_and_extension_0, output_folder_name
  );
  csv_obj.two_dimension_vector_to_csv(
   optimized_parameter, output_file_name + addition_and_extension_1, output_folder_name
  );
 }
 private:
 Data* data_p;
};


//main
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

/* コンパイル： chmod 700 compile.sh; ./compile.sh */

/* 引数    ：   in/ratio_0.35_record_no_0_r_peak_no_1.csv 
                in/combination_001.csv 
                in/nlopt_initial_001.csv 
                out
*/

int main(int argc, char* argv[]){
 //const int comb = 0;

 //分布関数毎のパラメータ数 0:正規分布=3, 1:対数正規分布=4
 //std::vector<int> fitting_function_parameter_num_vector = { 3,4 };

 //sekiguchi
 std::vector<int> fitting_function_parameter_num_vector = { 4,4 };

if(argc<5){
 std::cout << "./a.out in/waveform.csv in/fittingfunction in/nloptinit out_dir\n";
 exit(-1);
}

 /* Csvのインスタンス作成 */
 Csv csv_obj;

 /* check output directory */
 std::string output_folder_name = argv[4];
 if (!csv_obj.is_exist_folder(output_folder_name)) {
  std::cout << "出力のフォルダ設定エラー" << std::endl;
  exit(-1);
 }

 /* argv1: 12誘導のデータ を格納した.csv */
 std::string csv_file_name = argv[1];

 /* argv2: フィッティングする関数の組み合わせを格納した.csv */
 std::string input_combination_list_csv_name = argv[2];
 std::vector<std::vector<int>> combination_matrix;
 combination_matrix = csv_obj.read_csv<int>(input_combination_list_csv_name);

 /* 確率分布のパラメータの数の総和 */
 int max_parameter_num
  = get_max_parameter_num(combination_matrix[0], fitting_function_parameter_num_vector);


/* argv3: NLoptの初期値を格納した.csv */
 std::string input_initial_value_list_csv_name = argv[3];
 std::vector<std::vector<double>> nlopt_initial_value_matrix;
 nlopt_initial_value_matrix = csv_obj.read_csv<double>(input_initial_value_list_csv_name);

 /* ファイル名の確認 */
 std::cout << "12誘導心電図時系列データ：" << csv_file_name << std::endl;
 std::cout << "関数インデックスリスト：" << input_combination_list_csv_name << std::endl;
 std::cout << "NLoptの初期値ファイル：" << input_initial_value_list_csv_name << std::endl;

 /* フィッティング関数の確認 */
 std::cout << "関数インデックスリスト：";
 for (int i = 0; i < combination_matrix.at(0).size(); i++) {
  std::cout << combination_matrix[0][i] << " ";
 }
 std::cout << std::endl;

 /* 12誘導 */
 int data_channel_num = 12; //ECG_CHANNEL_NUM;

 /* P Q R S T の 5つ とは限らない */
 int distinct_point_num = combination_matrix.at(0).size();

 /* .csvのECGデータのステップサイズ(サンプリング) */
 double step_size = 0.002;//STEP_SIZE;

 /* 相対許容誤差 */
 double nlopt_ftol = 0.00001; //NLOPT_RELATIVE_TOLERANCE_ON_FUNCTION_VALUE;

 /* 各クラスで共有するデータ構造体 */
 Data* data_p = new Data();

 data_p->output_folder_name = output_folder_name;
 data_p->data_channel_num = data_channel_num;
 data_p->distinct_point_num = distinct_point_num;
 data_p->step_size = step_size;
 data_p->combination_matrix = combination_matrix;

 data_p->max_parameter_num = max_parameter_num;

 data_p->nlopt_initial_value_matrix = nlopt_initial_value_matrix;

 data_p->fitting_function_parameter_num_vector 
  = fitting_function_parameter_num_vector;
 data_p->csv_file_name = csv_file_name;
 data_p->nlopt_ftol = nlopt_ftol;
 data_p->output_csv_name_addition_and_extension_0 = std::string("_optimized.csv");
 data_p->output_csv_name_addition_and_extension_1 = std::string("_parameters.csv");

 

 /* データ構造体のメンバ変数のvectorのサイズを初期化 */
 data_p->initialize_vector_size(
  data_channel_num, distinct_point_num, max_parameter_num);


 /* ------------------フィッティング処理 開始----*/
 Process_for_one_csv process_for_one_csv_obj(data_p);
 Process_for_one_combination process_for_one_combination_obj(data_p);
 std::cout << "ファイル : " << csv_file_name << " を処理中 ... " << std::endl;

 /* 一つの .csv についての処理 */
 process_for_one_csv_obj.process();

 /* 誘導の数だけ繰り返す */
 for (int channel = 0; channel < data_channel_num; channel++) {
  /* フィッティング対象の誘導を意味するインデックスを格納 */
  data_p->void_pointer.channel_index = channel;
  /* Ⅲ, aVR, aVL, aVF は合成する誘導なのでフィッティングしない */
  if (2 <= channel && channel <= 5) {
   continue;
  }

  /* 関数の組み合わせリストについての処理 */
  process_for_one_combination_obj.process(channel);
 }

 /* フィッティング処理 終了 */
 std::cout << "最適化したパラメータで波形生成と.csv出力 開始\n";

 /* フィッティング後の処理のためのオブジェクト */
 Process_for_post_optimization process_for_post_optimization_obj(data_p);
 process_for_post_optimization_obj.generate_waveforms();

 //無事に終わったら・・・
 std::cout << "最適化したパラメータで波形生成と.csv出力 終了\n";
 delete data_p;
 return 0;
}
