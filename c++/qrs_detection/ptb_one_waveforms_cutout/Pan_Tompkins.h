#include <cstdio>
#include <iostream>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <iterator>

#ifndef PAN_TOMPKINS
#define PAN_TOMPKINS
typedef double dataType;


class Pantompkins{

 private:
 int WINDOWSIZE; // Integrator window size, in samples. The article recommends 150ms. So, FS*0.15.
                 // However, you should check empirically if the waveform looks ok.
 int NOSAMPLE;   // An indicator that there are no more samples to read. Use an impossible value for a sample.
 int FS;         // Sampling frequency.
 int BUFFSIZE;   // The size of the buffers (in samples). Must fit more than 1.66 times an RR interval, which
                 // typically could be around 1 second.
 int DELAY;      // Delay introduced by the filters. Filter only output samples after this one.
                 // Set to 0 if you want to keep the delay. Fixing the delay results in DELAY less samples
                 // in the final end result.
 
 //FILE *fin, *fout;       // Remove them if not using files and <stdio.h>.
 
 std::vector<dataType> internal_vector;
 int c_internal_vector;
 std::vector<bool> results_bool;
 std::vector<int> results_index;


 public:

  Pantompkins();//コンストラクタ1
  Pantompkins(int, int, int, int, int, bool);//コンストラクタ2
  
  void set_vector(std::vector<dataType>&);  /*  アルゴリズムに入力する vector をセットするメソッド  */
  void exec();                              /*  アルゴリズムを実行するメソッド                      */


  std::vector<bool> get_results_bool();     /*  結果を得るメソッド                                                                 */
                                            /*  set_vector で入力した vector と同じサイズの vector を出力                          */
                                            /*  アルゴリズムがRピークと判定したインデックスに true そうでないインデックスに false  */
   
  std::vector<dataType> get_results_peak_plot(); /*  結果を得るメソッド */
                                                 /*  set_vector で入力した vector と同じサイズの vector を出力                                                 */
                                                 /*  アルゴリズムがRピークと判定したインデックスに set_vector で入力した vectorの値 そうでないインデックスに 0 */
                                                 /*  set_vector で入力した vector と同じ vector を引数に入力すること                                           */

  std::vector<int> get_results_index();     /*  結果を得るメソッド  */
                                            /*  アルゴリズムがRピークと判定した回数と同じサイズの vector を出力    */
                                            /*  アルゴリズムがRピークと判定したインデックスを小さい順に順番に格納  */


  bool FILEorVECTOR;

  //void open_file(const char file_in[], const char file_out[]);
  //void close_file();
  //dataType input_file();
  
  dataType input_vector();
  //void output_file(int);
  
  void output_vector(int);
  
};

#endif
