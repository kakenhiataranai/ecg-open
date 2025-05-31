

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



int main(int argc, char* argv[]) {

 //if(argc<3){
 // printf("Usage: ./a.out inputfile outputfile\n");
 //}
 //else{

 std::string filename = "PTB_control_12_lead_ecg_extracted_sampling_500.csv";
 
 std::vector<std::vector<double>> input_data_hako = readCSV(filename);
 std::cout << input_data_hako.size() << std::endl;
 std::cout << input_data_hako.at(0).size() << std::endl;


 int ptb_control_no = 6;   /* PTB control 群から被験者を選択 1 ～ 80 */


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

 Pantompkins P(window_size, sampling_frequency, buffer_size, no_sample, delay, true);


 /* 取り込んだ .csv から Pan Tompkins にインプットするデータを抽出 */
 std::vector<double> input_data(input_data_hako.size()); 
 for (int i = 0; i < input_data.size(); i++) {
  input_data[i] = input_data_hako[i][1 + 12 * ptb_control_no];  /* Ⅱ誘導なのでインデックスは 1 */
                                                                /* 心電図のchannel数は 12       */
 }

 P.set_vector(input_data);

 //if(P.FILEorVECTOR){
 // P.open_file(argv[1],argv[2]);
 //}
 
 P.exec();

 //if(P.FILEorVECTOR){
 // P.close_file();
 //}
 

 std::vector<bool> results_bool = P.get_results_bool();
 std::cout << results_bool.size() << std::endl;

 std::vector<dataType> results_peak_plot = P.get_results_peak_plot();
 std::cout << results_peak_plot.size() << std::endl;

 std::vector<int> results_index = P.get_results_index();
 std::cout << results_index.size() << std::endl;
  

 //for (int i = 0; i < results_peak_plot.size(); i++) {
 // std::cout << results_peak_plot[i] << std::endl;
 //}

 for (int i = 0; i < results_index.size(); i++) {
  std::cout << results_index[i] << std::endl;
 }


 /* .txt に出力 */
 int step_num = input_data.size();
 int result_column_num = 2;
 std::string filename_result = "result.txt";
 std::ofstream outputfile(filename_result);
 for (int i = 0; i < step_num; i++) {
  outputfile << input_data[i] << "\t";
  outputfile << results_peak_plot[i] << "\t";
  outputfile << "\n";  
 }
 outputfile.close();

 
 return 0;
}
