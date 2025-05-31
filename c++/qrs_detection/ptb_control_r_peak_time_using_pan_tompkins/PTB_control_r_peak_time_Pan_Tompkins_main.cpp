

#include "Pan_Tompkins.h"

#include <iostream>
#include <fstream>
#include <sstream>



/*                            */
/* .csv を読み込むための関数  */
/*                            */
std::vector<std::vector<double>> readCSV(const std::string& filename) {
 std::vector<std::vector<double>> data;
 std::ifstream file(filename);

 if (!file.is_open()) {
  std::cerr << "Failed to open file: " << filename << std::endl;
  return data;
 }

 std::string line;
 while (std::getline(file, line)) {
  std::vector<double> row;
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



/* 本プログラムの出力で得られた r peak の時間は                     */
/* delay の設定値を 0 にしているため実際の peak より若干遅れています        */
/* 差分をとってRR間隔で考えるという用途であれば問題ないと思われます */
/* 開始 10[sec]未満のrピークは捨てます */
/* 最後のrピークは捨てます */

int main(int argc, char* argv[]) {


/*                 */
/* .csv を取り込み */
/*                 */
 std::vector<std::vector<double>> input_data_hako = readCSV("PTB_control_12_lead_ecg_extracted_sampling_500.csv");


 int ptb_control_num = 80;       /* PTB control群は 80レコード */
                                 /* 重複ありなので 80人 ではない */
  
 int start_threshold = 5000;     /* 開始 10[sec] より 0 に近いところのピークは捨てる サンプリング 500[Hz] なので 5000 */
                                 /* (最後のピークも捨てる) */

 double step_size = 0.002;       /* ステップサイズ 0.002[sec] */


 int window_size = 75;          /* 論文は150[ms]を推奨 サンプリング0.002[sec]だと75 */
 int sampling_frequency = 500;  /* サンプリング0.002[sec] */
 int buffer_size = 1000;        /* 2[sec]あれば十分だろうから サンプリング0.002[sec]だと1000 */
 int no_sample = -32000;
 int delay = 0;                /* 1/ 360 /0.002 に何となく近い値で 30 */
                               /* と思ったが検出したピークを前にスライドさせない 0 が自然かなと */
                               

//#define WINDOWSIZE 20   // Integrator window size, in samples. The article recommends 150ms. So, FS*0.15.
//                        // However, you should check empirically if the waveform looks ok.
//#define NOSAMPLE -32000 // An indicator that there are no more samples to read. Use an impossible value for a sample.
//#define FS 360          // Sampling frequency.
//#define BUFFSIZE 600    // The size of the buffers (in samples). Must fit more than 1.66 times an RR interval, which
//                        // typically could be around 1 second.
//
//#define DELAY 22		  // Delay introduced by the filters. Filter only output samples after this one.
//                        // Set to 0 if you want to keep the delay. Fixing the delay results in DELAY less samples
//                        // in the final end result.



 std::vector<std::vector<int>> peak_index_matrix(ptb_control_num);
 std::vector<int> peak_index_num(ptb_control_num, 0);

 int max_index_truncated_num = 0;


 /* PTB control群の被験者 0 ～ 79 について処理 */
 for (int ptb_control_no = 0; ptb_control_no < ptb_control_num; ptb_control_no++) {

  /* 取り込んだ .csv から Pan Tompkins にインプットするデータを抽出 */
  std::vector<double> input_data(input_data_hako.size());
  for (int i = 0; i < input_data.size(); i++) {
   input_data[i] = input_data_hako[i][1 + 12 * ptb_control_no];  /* Ⅱ誘導なのでインデックスは 1 */
                                                                 /* 心電図のchannel数は 12       */
  }

  Pantompkins* P = new Pantompkins(window_size, sampling_frequency, buffer_size, no_sample, delay, true);

  /* Pan Tompkins を実行 */
  P->set_vector(input_data);
  P->exec();
  
  /* 検出したピークのインデックスを取得 */
  std::vector<int> results_index = P->get_results_index();
  int index_num = results_index.size();

  for (int i = 0; i < index_num; i++) {
   /* 開始 10[sec] より 0 に近いところのピークは捨てる */

   if (results_index[i] < start_threshold) {
    continue;
   }
   /* 最後のピークは捨てる */
   else if (i == index_num - 1) {
    continue;
   }
   
   /* それ以外を格納 */
   peak_index_matrix[ptb_control_no].push_back(results_index[i]);

   /* インデックス数をインクリメント */
   peak_index_num[ptb_control_no]++;
  }

  /* 最大値を更新 */
  if (max_index_truncated_num < peak_index_matrix[ptb_control_no].size()) {
   max_index_truncated_num = peak_index_matrix[ptb_control_no].size();
  }

  delete P;
 }



 /*             */
 /* .csv に出力 */
 /*             */
 std::ofstream outputfile("result_r_peak_time.csv");

 /* 縦方向 */
 for (int record_no = 0; record_no < ptb_control_num; record_no++) {

  /* 横方向 */
  for (int peak_cnt = 0; peak_cnt < max_index_truncated_num; peak_cnt++) {
   if (peak_index_num[record_no] < peak_cnt) {
    outputfile << 0;
   }
   else {
    outputfile << peak_index_matrix[record_no][peak_cnt] * step_size;
   }
   if (peak_cnt < max_index_truncated_num - 1) {
    outputfile << ",";
   }
  }
  outputfile << "\n";
 }
 outputfile.close();
 
 return 0;
}
