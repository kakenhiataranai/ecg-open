

#include "Pan_Tompkins.h"

#include <iostream>
#include <fstream>
#include <sstream>


const int ecg_channel_num = 12;


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



void output_csv_cut_out_wave(int record_num,
                            std::vector<std::vector<double>> input_data_csv,
                            std::vector<std::vector<int>> peak_index_matrix,
                            std::vector<int> mean_rr_interval_index_num) {

 std::vector<std::vector<int>> start_side_index_matrix;
 std::vector<std::vector<int>> end_side_index_matrix;
 start_side_index_matrix.resize(record_num);
 end_side_index_matrix.resize(record_num);
 

 std::vector<double> rate_vector{ 0.35, 0.4, 0.45, 0.5};
 std::vector<std::string> rate_vector_str{ "0.35", "0.40", "0.45", "0.50" };
 int rate_num = rate_vector.size();


 //std::cout << "input_data_csv.size() = " << input_data_csv.size() << std::endl;
 //std::cout << "peak_index_matrix.size() = " << peak_index_matrix.size() << std::endl;
 //std::cout << "mean_rr_interval_index_num.size() = " << mean_rr_interval_index_num.size() << std::endl;
 //std::cout << "rate_vector.size() = " << rate_vector.size() << std::endl;



 /* 割合で繰り返し */
 for (int rate_cnt = 0; rate_cnt < rate_num; rate_cnt++) {

  /* レコードで繰り返し */
  for (int record_cnt = 0; record_cnt < record_num; record_cnt++) {

   /* あるレコードについて切り出すインデックス数は    (後で書く！)       */
   int cut_out_index_num = mean_rr_interval_index_num[record_cnt];

   std::cout << "cut_out_index_num  = " << cut_out_index_num << std::endl;

   /* あるレコードについてピークの前側のインデックス数とピークの後側のインデックス数を得る */
   int start_side_index_num = cut_out_index_num * rate_vector[rate_cnt];
   int end_side_index_num = cut_out_index_num - start_side_index_num;

   /* ピークのインデックス列の各要素に対して前側のインデックス列と後側のインデックス列を得るためのfor文 */
   /* あるレコードについて切り出す位置がすべて決まる                                                    */
   int r_peak_num = peak_index_matrix[record_cnt].size();
   for (int i = 0; i < r_peak_num; i++) {
    int r_peak_index = peak_index_matrix[record_cnt][i];
    int start_size_index = r_peak_index - start_side_index_num - 1;
    int end_size_index = r_peak_index + end_side_index_num;
    start_side_index_matrix[record_cnt].push_back(start_size_index);
    end_side_index_matrix[record_cnt].push_back(end_size_index);


    //std::cout << start_size_index << "   " << r_peak_index << "   " << end_size_index << "   " << end_size_index - start_size_index << std::endl;


   }

   /* あるレコードについてピークごとに繰り返し */
   for (int peak_cnt = 0; peak_cnt < r_peak_num; peak_cnt++) {
    int tmp_peak_index = peak_index_matrix[record_cnt][peak_cnt];

    /* サイズ = 切り出すインデックス数 × 12 */
    std::vector<std::vector<double>> cut_out_data_matrix(cut_out_index_num, std::vector<double>(ecg_channel_num));

    /* 切り出しデータ抽出 */

    //std::cout << "start_side_index_matrix[record_cnt][peak_cnt]  = " << start_side_index_matrix[record_cnt][peak_cnt] << std::endl;
    //std::cout << "end_side_index_matrix[record_cnt][peak_cnt]  = " << end_side_index_matrix[record_cnt][peak_cnt] << std::endl;
    //int extract_num = end_side_index_matrix[record_cnt][peak_cnt] - start_side_index_matrix[record_cnt][peak_cnt] - 1;
    //std::cout << "extract_num  = " << extract_num << std::endl;

    //for (int i = start_side_index_matrix[record_cnt][peak_cnt]; i < end_side_index_matrix[record_cnt][peak_cnt]; i++) {
    for (int i = 0; i < cut_out_index_num; i++) {
     for (int c = 0; c < ecg_channel_num; c++) {
      int start_index = start_side_index_matrix[record_cnt][peak_cnt];
      cut_out_data_matrix[i][c] = input_data_csv[start_index + i][ecg_channel_num * record_cnt + c];
     }
    }

    /* あるレコードのあるピークについて切り出した12個の波形を.csv出力 */
    std::string filename = "ratio_" + rate_vector_str[rate_cnt] + "_record_no_" + std::to_string(record_cnt) +"_r_peak_no_" + std::to_string(peak_cnt) + ".csv";
    std::ofstream file(filename);
    if (file.is_open()) {

    /* 行方向は 切り出すインデックス数 */
     for (int i = 0; i < cut_out_data_matrix.size(); i++) {
      /* 列方向は 12 */
      for (int j = 0; j < ecg_channel_num; j++) {
       file << cut_out_data_matrix[i][j];
       if (j != ecg_channel_num - 1) {
        file << ",";
       }
      }
      file << "\n";
     }
     file.close();
     std::cout << "ファイル " << filename << " にデータを出力しました。\n";
    }
    else {
      std::cout << "ファイル " << filename << " を開けませんでした。\n";
    }
    


    ///* .csv出力 */    
    //for (int i = 0; i < cut_out_data_matrix.size(); i++) {
    // std::string filename = "result_" + rate_vector_str[rate_cnt] + "_r_peak_no_" + std::to_string(peak_cnt) + ".csv";
    // std::ofstream file(filename);
    // if (file.is_open()) {
    //  for (int j = 0; j < cut_out_data_matrix[i].size(); j++) {
    //   file << cut_out_data_matrix[i][j];
    //   if (j != cut_out_data_matrix[i].size() - 1) {
    //    file << ",";
    //   }
    //  }
    //  file.close();
    //  std::cout << "ファイル " << filename << " にデータを出力しました。\n";
    // }
    // else {
    //  std::cout << "ファイル " << filename << " を開けませんでした。\n";
    // }
    //}

   }

  }
 }
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
 std::vector<std::vector<double>> input_data_hako = readCSV("PTB_all_12_lead_ecg_extracted_sampling_500_hz.csv");
 //std::vector<std::vector<double>> input_data_hako = readCSV("PTB_control_12_lead_ecg_extracted_sampling_500_hz.csv");
 //std::vector<std::vector<double>> input_data_hako = readCSV("PTB_control_three_record_12_lead_ecg_extracted_sampling_500_hz.csv");


 int ptb_num = 549;              /* PTB は全部で 549レコード */
                                 /* 重複ありなので 549人 ではない */

 //int ptb_num = 3;   // debug



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



 std::vector<std::vector<int>> peak_index_matrix(ptb_num);
 std::vector<int> peak_index_num(ptb_num, 0);

 int max_index_truncated_num = 0;


 /* PTB control群の被験者 0 ～ 549 について処理 */
 for (int ptb_record = 0; ptb_record < ptb_num; ptb_record++) {

  /* 取り込んだ .csv から Pan Tompkins にインプットするデータを抽出 */
  std::vector<double> input_data(input_data_hako.size());
  for (int i = 0; i < input_data.size(); i++) {
   input_data[i] = input_data_hako[i][1 + 12 * ptb_record];  /* Ⅱ誘導なのでインデックスは 1 */
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
   /* 最後の1つのピークは捨てる */
   else if (i >= index_num - 1) {
    continue;
   }

   /* それ以外を格納 */
   peak_index_matrix[ptb_record].push_back(results_index[i]);

   /* インデックス数をインクリメント */
   peak_index_num[ptb_record]++;
  }

  /* 最大値を更新 */
  if (max_index_truncated_num < peak_index_matrix[ptb_record].size()) {
   max_index_truncated_num = peak_index_matrix[ptb_record].size();
  }

  delete P; 

 }


 std::cout << "peak_index_matrix 行の要素数 " << peak_index_matrix.size() <<std::endl;
 std::cout << "peak_index_matrix 列の要素数" << peak_index_matrix.at(0).size() << std::endl;


 ///*                             */
 ///* 検出したピークを.csv に出力 */
 ///*                             */
 ////std::ofstream outputfile("result_r_peak_time.csv");
 //std::ofstream outputfile("result_r_peak_index.csv");

 ///* 縦方向 */
 //for (int record_no = 0; record_no < ptb_num; record_no++) {

 // /* 横方向 */
 // for (int peak_cnt = 0; peak_cnt < max_index_truncated_num; peak_cnt++) {
 //  if (peak_index_num[record_no] <= peak_cnt) {
 //   outputfile << 0;
 //  }
 //  else {
 //   //outputfile << peak_index_matrix[record_no][peak_cnt] * step_size;
 //   outputfile << peak_index_matrix[record_no][peak_cnt];
 //  }

 //  if (peak_cnt < max_index_truncated_num - 1) {
 //   outputfile << ",";
 //  }
 // }
 // outputfile << "\n";
 //}
 //outputfile.close();



 /*                    */
 /* RR-interval を計算 */
 /*                    */
 /* PTB control群の被験者 0 ～ 549 について処理 */


 /* 各レコードに対する rr 間隔の平均 */
 std::vector<double> mean_rr_interval(ptb_num);

 /* 各レコードに対する rr 間隔のインデックス数の平均 */
 std::vector<int> mean_rr_interval_index_num(ptb_num);


 /* 縦方向 */
 for (int record_no = 0; record_no < ptb_num; record_no++) {

  double tmp_interval = 0;
  int tmp_index_num = 0;
  int rr_cnt = 0;

  /* 横方向 */
  for (int column_cnt = 0; column_cnt < peak_index_num[record_no]; column_cnt++) {

   /* 差分をとるので一番最初はスキップ */
   if (column_cnt == 0) {
    continue;
   }

   tmp_interval += (peak_index_matrix[record_no][column_cnt] - peak_index_matrix[record_no][column_cnt - 1]) * step_size;
   tmp_index_num += peak_index_matrix[record_no][column_cnt] - peak_index_matrix[record_no][column_cnt - 1];
   ++rr_cnt;
  }

  mean_rr_interval[record_no] = tmp_interval / rr_cnt;
  mean_rr_interval_index_num[record_no] = tmp_index_num / rr_cnt;
 }

 ///*                             */
 ///* 平均rr間隔を.csv に出力     */
 ///*                             */
 //std::ofstream outputfile_1("result_mean_rr_interval.csv");

 ///* 縦方向 */
 //for (int record_no = 0; record_no < ptb_num; record_no++) {
 // outputfile_1 << mean_rr_interval[record_no];
 //  //outputfile << ","; 
 // outputfile_1 << "\n";  
 //}
 //outputfile_1.close();

 output_csv_cut_out_wave(ptb_num, input_data_hako, peak_index_matrix, mean_rr_interval_index_num);

 
 return 0;
}
