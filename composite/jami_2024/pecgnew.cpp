#include <libconfig.h++>
#include "C.h"
#include "C-stat.h"
#include "C-nlopt.h"
#include "pecgnew.h"


// -----------------------------------------------------------------------------
// .conf の設定と出力
// -----------------------------------------------------------------------------
// debug = 0
//   パラメータリストを出力
//
// debug = 1
//   パラメータリストを出力
//   中間ファイルを出力
//
// debug = 2
//   パラメータリストを出力
//   中間ファイルを出力
//   画面出力
// -----------------------------------------------------------------------------
// gnuplot = 0
//   何も出力しない
//
// gnuplot = 1
//   プロット用の中間ファイルを出力
//    pecgnew_plot.gnu を実行
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// 最適化の初期値の決め方
// -----------------------------------------------------------------------------
// a : 切り出し時系列データの最大値 / 1.6
// 1.6 は a=1，b=0，c=0.25 のときの対数正規型関数のグラフの最大値付近の値

// 説明：定義より a は valgaussian において定数倍を意味する係数である．
// よって，最適化の結果得られる a は，フィッティング対象の心電図データのピークの値が
// valgaussian で a = 1 とした場合のピークの値の何倍になるか，を意味する値になるはずである．
// よって，前者を分子に，後者を分母に設定することで，最適化の結果得られる a に近い値となるはずであり，
// その値を a の初期値として設定する．

// -----------------------------------------------------------------------------
// b :（切り出し時系列データの開始時間 + 切り出し時系列データの終了時間）/ 2 - 1.0
// 1.0 は a=なんでも，b=0，c=0.25 のときの対数正規型関数のグラフのピーク付近の横軸の値

// 説明：b は対数正規型関数の横方向をどの程度シフトするのか，そのシフト幅を意味する係数である．
// 対数正規型関数は(mu=0を暗黙的に仮定しているので)山のピークは横軸 x = 1 付近である．
// したがってフィッティング対象の切り出し心電図データのピークの横軸の中心付近から 1 を引くことで，
// 対数正規型関数が x - b = 0 を満たす x を導出することができる（b = x として bが得られる）．
// なお，フィッティング対象の切り出し心電図データのピークの横軸の中心付近は概ねデータの中点付近であると仮定して，
//（切り出し時系列データの開始時間 + 切り出し時系列データの終了時間）/ 2  で求める．

// T波などの左右対称の場合については左右反転させるので以下の式にした．
// b :（切り出し時系列データの開始時間 + 切り出し時系列データの終了時間）/ 2 + 1.0

// -----------------------------------------------------------------------------
// c : min(0.5, max(0.2835 * ratio^(-0.566), 0.1));
// ratio = 切り出し波形の最大値 / 切り出し波形の時間幅;

// 説明：対数正規型関数でフィッティングする際，基本的にガウス関数のような山型？でフィッティングすることが想定されている．
// 切り出し波形の最大値と切り出し波形の時間幅から算出する ratio を回帰式にあてはめて当該切り出し波形のsigma を求める．

// 回帰式の求め方は以下である．
// 対数正規分布の式（mu=0）について，山の最大値と山の幅の比を ratio とし，ratio と sigma の関係の回帰曲線を求める．
// 具体的には，横軸 ratio で縦軸 sigma として，sigma 0.1, 0.2, 0.3, 0,4, 0,5 それぞれのときについてratioをプロットし，
// Excel の曲線あてはめの機能を用いると y = 0.2835 x^(-0.566) となる．

// 回帰曲線の外側（特にsigmaが小さい方）で値が小さくなりすぎたり大きくなりすぎることを避けるため
// 上限を 0.5とする
// 下限を 0.1 とする

// なお，山の幅を求める際には，両側とも閾値 y=0.05 付近の x を用いる
//
// -----------------------------------------------------------------------------
// 参考
//  valloggaussian = a / (x-b) × exp(-ln(x-b) × ln(x-b) / (2 × c × c ));
// -----------------------------------------------------------------------------


// 暫定でここに書く
class Data {
public:
 int parameter_count;
 std::vector<double> lb;
 std::vector<double> ub;
 std::vector<double> init;
 
 std::string release_observed_x_axis;
 std::string release_observed_y_axis;
 std::string release_fitted_x_axis;
 std::string release_fitted_y_axis;
 std::string release_optimized_params;
 std::string release_is_not_flip_loggausian;
 std::string debug_org_plus;
 std::string debug_org_plus_index;
 std::string debug_org_minus;
 std::string debug_org_minus_index;
 std::string debug_sorted_org_plus;
 std::string debug_sorted_org_plus_index;
 std::string debug_sorted_org_minus;
 std::string debug_sorted_org_minus_index;
 std::string debug_init_plus;
 std::string debug_init_minus;
 std::string debug_lb;
 std::string debug_ub;
 
 std::vector<double> time_series;
 std::vector<int> time_series_index;
 std::vector<double> x_axis;

 double optimal_value;

 // vectoroutvで出力可能となるよう(boolではなく)int型にする
 std::vector<int> is_not_flip_loggausian;

 Data(Config *cf) {
  parameter_count = parameter_count_table(cf->func_type);
  lb.resize(parameter_count);
  lb[0] = cf->lb_0;
  lb[1] = cf->lb_1;
  lb[2] = cf->lb_2;
  ub.resize(parameter_count);
  ub[0] = cf->ub_0;
  ub[1] = cf->ub_1;
  ub[2] = cf->ub_2;

  init.resize(parameter_count);

  release_observed_x_axis = cf->release_base_name + cf->ext_org_x_axis;
  release_observed_y_axis = cf->release_base_name + cf->ext_org_y_axis;
  release_fitted_x_axis = cf->release_base_name + cf->ext_fit_x_axis;
  release_fitted_y_axis = cf->release_base_name + cf->ext_fit_y_axis;
  release_optimized_params = cf->release_base_name + cf->ext_optimized_parameters;
  release_is_not_flip_loggausian = 
   cf->release_base_name + cf->ext_is_not_flip_loggausian;

  debug_org_plus = cf->debug_base_name + cf->ext_org_plus;
  debug_org_plus_index = cf->debug_base_name + cf->ext_org_plus_index;
  debug_org_minus = cf->debug_base_name + cf->ext_org_minus;
  debug_org_minus_index = cf->debug_base_name + cf->ext_org_minus_index;
  debug_sorted_org_plus = cf->debug_base_name + cf->ext_sorted_org_plus;
  debug_sorted_org_plus_index = cf->debug_base_name + cf->ext_sorted_org_plus_index;
  debug_sorted_org_minus = cf->debug_base_name + cf->ext_sorted_org_minus;
  debug_sorted_org_minus_index = cf->debug_base_name + cf->ext_sorted_org_minus_index;

  debug_init_plus = cf->debug_base_name + cf->ext_init_plus;
  debug_init_minus = cf->debug_base_name + cf->ext_init_minus;
  debug_lb = cf->debug_base_name + cf->ext_lb;
  debug_ub = cf->debug_base_name + cf->ext_ub;
 }
};

/*
double model_function_valgaussian(const std::vector<double>& model_func_parameter, const double x) {
 Statistics s;
 const double a = model_func_parameter[0];
 const double b = model_func_parameter[1];
 const double c = model_func_parameter[2];
 return s.valgaussian(a, b, c, x);
}
*/

double model_function_valloggaussian(const std::vector<double>& model_func_parameter, const double x) {
 Statistics s;
 const double a = model_func_parameter[0];
 const double b = model_func_parameter[1];
 const double c = model_func_parameter[2];
 return s.valloggaussian(a, b, c, x);
}

double model_function_valloggaussian_line_symmetry(const std::vector<double>& model_func_parameter, const double x) {
 Statistics s;
 const double a = model_func_parameter[0];
 const double b = model_func_parameter[1];
 const double c = model_func_parameter[2];
 return s.valloggaussian_line_symmetry(a, b, c, x);
}


double objective_function(const std::vector<double>& parameter,
 std::vector<double>& parameter_grad, void* function_and_data) {

 // 使える型にする
 Data* data = static_cast<Data*> (function_and_data);

 std::vector<double> y = data->time_series;
 std::vector<double> x = data->x_axis;
 
 double res = residual_sum_of_squares(model_function_valloggaussian, parameter, y, x);
 
 // 最適値を格納
 data->optimal_value = res;
 
 return res;
 //return residual_sum_of_squares(model_function_valloggaussian, parameter, y, x);
}

double objective_function_flip(const std::vector<double>& parameter,
 std::vector<double>& parameter_grad, void* function_and_data) {

 // 使える型にする
 Data* data = static_cast<Data*> (function_and_data);

 std::vector<double> y = data->time_series;
 std::vector<double> x = data->x_axis;
 
 double res = residual_sum_of_squares(model_function_valloggaussian_line_symmetry, parameter, y, x);
 
 // 最適値を格納
 data->optimal_value = res;
 
 return res;
}


//curveの面積を計算する: 大きな波を選択するところで使う
double get_curve_area(std::vector<double> &one_curve){
 double area=0.;
 for(auto i:one_curve){
  area+=i;
 }
 return area;
}
 
///// main

int main(int argc, char* argv[]) {
 if (argc < 2) { std::cout << "sample CONFIGFILE\n"; exit(-1); }
 C c;
 Statistics s;
 Config cf(argv[1], &c);

 Data d(&cf);


 // 波形取り込み
 std::string data = binload(cf.input_filename);
 std::vector<std::vector<double>> data2d = string2matrix<double>(data);

 // 誘導を指定
 std::vector<double> curve = pickcol(data2d, cf.lead);

 //------------------------------- データ前処理 --------------------------------------
 //中央値で調整
 double add_value = s.median(&curve); 
 std::vector<double> curve_adj = c.vectorp(curve, -add_value);

 //正負に分離して配列に格納
 std::vector<std::vector<double>> curve_plus = getplus(curve_adj);
 std::vector<std::vector<int>> curve_plus_index = getplusindex(curve_adj);
 std::vector<std::vector<double>> curve_minus = getminus(curve_adj);//正の値にして返すよう変更済 
 std::vector<std::vector<int>> curve_minus_index = getminusindex(curve_adj);
 

 // debug begin:デバッグモードの出力
 if (cf.debug == 1 || cf.debug == 2) {
  std::ofstream o_p(d.debug_org_plus.c_str(), std::ios::out);
  for (auto i : curve_plus) {
   vectorout(o_p, &i);
  }
  std::ofstream o_p_index(d.debug_org_plus_index.c_str(), std::ios::out);
  for (auto i : curve_plus_index) {
   vectorout(o_p_index, &i);
  }
  std::ofstream o_m(d.debug_org_minus.c_str(), std::ios::out);
  for (auto i : curve_minus) {
   vectorout(o_m, &i);
  }
  std::ofstream o_m_index(d.debug_org_minus_index.c_str(), std::ios::out);
  for (auto i : curve_minus_index) {
   vectorout(o_m_index, &i);
  }
 }
 // debug end


 //サイズとindexのペアを作って面積の小さい順に自動で並べる
 std::map<double,int> curve_plus_size_map;
 int index= 0;
 for(auto z:curve_plus){
  curve_plus_size_map[get_curve_area(z)] = index;
  index++;
 }

 std::map<double,int> curve_minus_size_map;
 index= 0;
 for(auto z:curve_minus){
  curve_minus_size_map[get_curve_area(z)] = index;
  index++;
 }

// フィッティングする時系列のインデックスを取り出す
 std::vector<int> selected_index_p;
 int cnt = 0; 
 for (auto iter = curve_plus_size_map.rbegin(); 
  cnt < std::min((int)curve_plus_size_map.size(), cf.plus_extract_count); ++iter) {
  selected_index_p.push_back((*iter).second);
  ++cnt;
 }

 std::vector<int> selected_index_m;
 cnt = 0;
 for (auto iter = curve_minus_size_map.rbegin(); 
  cnt < std::min((int)curve_minus_size_map.size(), cf.minus_extract_count); ++iter) {
  selected_index_m.push_back((*iter).second);
  ++cnt;
 }


 // 出力ファイル : 使用しない場合はプログラムの最後で削除
 std::ofstream o_sorted_p(d.debug_sorted_org_plus, std::ios::out);
 std::ofstream o_sorted_p_index(d.debug_sorted_org_plus_index, std::ios::out);
 std::ofstream o_sorted_m(d.debug_sorted_org_minus, std::ios::out);
 std::ofstream o_sorted_m_index(d.debug_sorted_org_minus_index, std::ios::out);
 std::ofstream o_optimized_params(d.release_optimized_params, std::ios::out); 
 std::ofstream o_is_not_flip_loggausian(
  d.release_is_not_flip_loggausian, std::ios::out);

 std::ofstream o_init_p(d.debug_init_plus, std::ios::out);
 std::ofstream o_init_m(d.debug_init_minus, std::ios::out);
 std::ofstream o_lb(d.debug_lb, std::ios::out);
 std::ofstream o_ub(d.debug_ub, std::ios::out);

 // 再構築用
 std::vector<std::vector<double>> optimized_params;
 std::vector<std::string> func_type_vec;
 
 std::vector<std::vector<int>> data_range_index;
 


 
 //------------------------------- 最適化計算 ---------------------------------------
 const double lognormal_vertex_point = 1.; // 対数正規分布グラフの頂点付近の横位置(mu=0のとき)
 const double logggausian_vertex_height = 1.6;

 const int line_symmetry_count = 2; // 左右の対称性についての変数



 // プラス側の大きい順にフィッティング
 for (auto i : selected_index_p) {
  d.time_series = curve_plus[i];
  d.time_series_index = curve_plus_index[i];
  d.x_axis = time_series_axis(d.time_series_index, cf.step_size);
  void* nlopt_void_ptr = static_cast<void*>(&d);
  
  double optimal_value = 0;
  double optimal_value_flip = 0;
  std::vector<double> parameter;
  std::vector<double> parameter_flip;

  // 左右の非対称性：ユークリッド距離の二乗が小さい方を選択
  for (int i = 0; i < line_symmetry_count; i++) {

   
   // 通常の対数正規型関数
   if (i == 0){

    // 初期値計算
    const double time_series_max = s.max(&(d.time_series));
    const double x_width = d.time_series.size() * cf.step_size;
    const double ratio = time_series_max / x_width;    
    d.init[0] = time_series_max / logggausian_vertex_height;
    d.init[1] = (d.x_axis[0] + d.x_axis.back()) / 2. - lognormal_vertex_point; // 切り出し波形の中点から 1.を引いた値
    d.init[2] = std::min(0.5, std::max(0.2835 * pow(ratio, -0.566), 0.1));

    d.ub[1] = d.x_axis[0] - cf.step_size; // 波形がはじまる直前まで
    d.lb[1] = cf.lb_1;

    // NLopt実行
    parameter = exec_nlopt(cf.algolithm, d.init, d.lb, d.ub, cf.is_relative, 
     cf.epsilon, nlopt_void_ptr, objective_function);

    optimal_value = d.optimal_value;
   }
   // 対称性が逆の対数正規型関数
   else if (i == 1) {

    // 初期値計算
    const double time_series_max = s.max(&(d.time_series));
    const double x_width = d.time_series.size() * cf.step_size;
    const double ratio = time_series_max / x_width;    
    d.init[0] = time_series_max / logggausian_vertex_height;
    d.init[1] = (d.x_axis[0] + d.x_axis.back()) / 2. + lognormal_vertex_point; // 切り出し波形の中点に 1.を加えた値
    d.init[2] = std::min(0.5, std::max(0.2835 * pow(ratio, -0.566), 0.1));

    d.ub[1] = cf.ub_1;
    d.lb[1] = d.x_axis.back() + cf.step_size; // 波形がおわる直後まで

    // NLopt実行
    parameter_flip = exec_nlopt(cf.algolithm, d.init, d.lb, d.ub, cf.is_relative, 
     cf.epsilon, nlopt_void_ptr, objective_function_flip);

    optimal_value_flip = d.optimal_value;
   }

  }

  if(optimal_value< optimal_value_flip){
   d.is_not_flip_loggausian.push_back(true);
   optimized_params.push_back(parameter);
   vectorout(o_optimized_params, &parameter);
  }
  else {
   d.is_not_flip_loggausian.push_back(false);
   optimized_params.push_back(parameter_flip);
   vectorout(o_optimized_params, &parameter_flip);
  }  
  
  func_type_vec.push_back(cf.func_type);
  data_range_index.push_back(d.time_series_index);
  
  // debug begin:デバッグモードの出力
  if (cf.debug == 1 || cf.debug == 2) {
   vectorout(o_sorted_p, &curve_plus[i]);
   vectorout(o_sorted_p_index, &curve_plus_index[i]);
   vectorout(o_init_p, &d.init);
   vectorout(o_lb, &d.lb);
   vectorout(o_ub, &d.ub);
  }
  if (cf.debug == 2) {
   std::cout << "-- " << i << "\nselected_index[i] : " << i << std::endl;
   std::cout << "d.init[0] : " << d.init[0] << std::endl;
   std::cout << "d.init[1] : " << d.init[1] << std::endl;
   std::cout << "d.init[2] : " << d.init[2] << std::endl;
   std::cout << "d.lb[0] : " << d.lb[0] << std::endl;
   std::cout << "d.lb[1] : " << d.lb[1] << std::endl;
   std::cout << "d.lb[2] : " << d.lb[2] << std::endl;
   std::cout << "d.ub[0] : " << d.ub[0] << std::endl;
   std::cout << "d.ub[1] : " << d.ub[1] << std::endl;
   std::cout << "d.ub[2] : " << d.ub[2] << std::endl;


   for (int j = 0; j < d.parameter_count; j++) {
	  //std::cout << "parameter[" << j << "] : " << parameter[j] << std::endl;   
   }
  }
  // debug end

 } // プラス側の大きい順にフィッティング


 // マイナス側の大きい順にフィッティング
 for (auto i : selected_index_m) {
  d.time_series = curve_minus[i];
  d.time_series_index = curve_minus_index[i];
  d.x_axis = time_series_axis(d.time_series_index, cf.step_size);
  void* nlopt_void_ptr = static_cast<void*>(&d);

  double optimal_value = 0;
   double optimal_value_flip = 0;
   std::vector<double> parameter;
   std::vector<double> parameter_flip;

  // 左右の非対称性：ユークリッド距離の二乗が小さい方を選択
  for (int i = 0; i < line_symmetry_count; i++) {

   // 通常の対数正規型関数
   if (i == 0){

    // 初期値計算
    const double time_series_max = s.max(&(d.time_series));
    const double x_width = d.time_series.size() * cf.step_size;
    const double ratio = time_series_max / x_width;    
    d.init[0] = time_series_max / logggausian_vertex_height;
    d.init[1] = (d.x_axis[0] + d.x_axis.back()) / 2. - lognormal_vertex_point; // 切り出し波形の中点から 1.を引いた値
    d.init[2] = std::min(0.5, std::max(0.2835 * pow(ratio, -0.566), 0.1));

    d.ub[1] = d.x_axis[0] - cf.step_size; // 波形がはじまる直前まで
    d.lb[1] = cf.lb_1;

    // NLopt実行
    parameter = exec_nlopt(cf.algolithm, d.init, d.lb, d.ub, cf.is_relative, 
     cf.epsilon, nlopt_void_ptr, objective_function);
    parameter[0] *= -1;

    optimal_value = d.optimal_value;
   }
   // 対称性が逆の対数正規型関数
   else if (i == 1) {

    // 初期値計算
    const double time_series_max = s.max(&(d.time_series));
    const double x_width = d.time_series.size() * cf.step_size;
    const double ratio = time_series_max / x_width;    
    d.init[0] = time_series_max / logggausian_vertex_height;
    d.init[1] = (d.x_axis[0] + d.x_axis.back()) / 2. + lognormal_vertex_point; // 切り出し波形の中点に 1.を加えた値
    d.init[2] = std::min(0.5, std::max(0.2835 * pow(ratio, -0.566), 0.1));

    d.ub[1] = cf.ub_1;
    d.lb[1] = d.x_axis.back() + cf.step_size; // 波形がおわる直後まで

    // NLopt実行
    parameter_flip = exec_nlopt(cf.algolithm, d.init, d.lb, d.ub, cf.is_relative, 
     cf.epsilon, nlopt_void_ptr, objective_function_flip);
    parameter_flip[0] *= -1;

    optimal_value_flip = d.optimal_value;
   }
  }


  if(optimal_value< optimal_value_flip){
   d.is_not_flip_loggausian.push_back(true);
   optimized_params.push_back(parameter);
   vectorout(o_optimized_params, &parameter);
  }
  else{
   d.is_not_flip_loggausian.push_back(false);
   optimized_params.push_back(parameter_flip);
   vectorout(o_optimized_params, &parameter_flip);   
  }
  
  func_type_vec.push_back(cf.func_type);
  data_range_index.push_back(d.time_series_index);

  // debug begin:デバッグモードの出力
  if (cf.debug == 1 || cf.debug == 2) {
   vectorout(o_sorted_m, &curve_minus[i]);
   vectorout(o_sorted_m_index, &curve_minus_index[i]);
   vectorout(o_init_m, &d.init);
   vectorout(o_lb, &d.lb);
   vectorout(o_ub, &d.ub);
  }
  // debug end

 }
 
 //------------------------------- 再構築処理 -------------------------------------
 const int last_index = curve.size();
 std::vector<double> fitted_y_axis;
 if (cf.reconstruct == 0) {
  fitted_y_axis = reconstruct_fitting_curve(optimized_params, func_type_vec, last_index, &cf);
 }
 else if (cf.reconstruct == 1) {
  fitted_y_axis = reconstruct_fitting_curve_exp(optimized_params, func_type_vec, last_index, &cf, data_range_index);
 }
 else if (cf.reconstruct == 2) {
  fitted_y_axis = reconstruct_fitting_curve_sym(optimized_params, func_type_vec, last_index, &cf, data_range_index,
  d.is_not_flip_loggausian);
 }
 
 // ガウシアンノイズ付加
 std::vector<double> final_output = add_noise(fitted_y_axis, cf.noise_stdev);

 //------------------------------- 出力処理 ---------------------------------------
 
 // 反転してないフラグの出力
 vectoroutv(o_is_not_flip_loggausian, &d.is_not_flip_loggausian);

 // gnuplot begin:プロットモードの出力
 if (cf.gnuplot) {

  std::ofstream o_observed_y_axis(d.release_observed_y_axis, std::ios::out);
  vectoroutv(o_observed_y_axis, &curve_adj);

  std::ofstream o_observed_x_axis(d.release_observed_x_axis, std::ios::out);
  const int o_observed_y_axis_size = curve_adj.size();
  for (int i = 0; i < o_observed_y_axis_size; i++) {
   o_observed_x_axis << i * cf.step_size << std::endl;
  }

  std::ofstream o_release_fitted_y_axis(d.release_fitted_y_axis, std::ios::out);
  vectoroutv(o_release_fitted_y_axis, &final_output);

  //x軸ファイルを出力（アドホック。pecgnew.hへ）
  std::ofstream o_fitted_x_axis(d.release_fitted_x_axis, std::ios::out);
  const int fitted_y_axis_size = fitted_y_axis.size();
  for (int i = 0; i < fitted_y_axis_size; i++) {
   o_fitted_x_axis << i * cf.step_size << std::endl;
  }

  // グラフ出力
  const int plot_code = system(cf.gnuplot_exe_filename.c_str());

  // debug begin:デバッグモードの出力
// plot_code 未使用を避けるために書く
  if (1 <= cf.debug) {
   std::cout << "plot_code : " << plot_code << std::endl;
  }
  // debug end

 }
 // gnuplot end

 //------------------------------- 終了処理 --------------------------------------


 if (cf.debug == 0) {
  std::remove(d.debug_sorted_org_plus.c_str());
  std::remove(d.debug_sorted_org_plus_index.c_str());
  std::remove(d.debug_sorted_org_minus.c_str());
  std::remove(d.debug_sorted_org_minus_index.c_str());
  std::remove(d.debug_init_plus.c_str());
  std::remove(d.debug_init_minus.c_str());
  std::remove(d.debug_lb.c_str());
  std::remove(d.debug_ub.c_str());
 }

 return 0;
}
