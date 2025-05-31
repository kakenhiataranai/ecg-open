// generate_ecg_McSharry_main.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>
#include <fstream>

#include "ecg_McSharry_model_using_fftw.h"

int main()
{
	/* 計算パラメータ作成 ここから */
	double step_size = 0.002;   /* [sec] */
	int step_num_per_sec = 500; /* = 1 / 0.002  */
	int step_num = 5000;        /* シミュレーションステップ数  */
	int result_column_num = 5;
	std::vector<std::vector<double>> result(step_num, std::vector<double>(result_column_num));  /* サイズは (step_num × 4) */
	/* ここまで */


	/* 心電図の生成 計算実行 ここから */
	ecg_signals ecg_obj(step_size, step_num, step_num_per_sec);
	ecg_obj.generate_ecg_signal_one_cycle(result);	
	/* ここまで */


	
	/* テキストファイルに書き込みして出力 ここから */
	std::string filename = "result.txt";  /* t, x, y, z (ecg), omega */
	std::ofstream outputfile(filename);
	for (int i = 0; i < step_num; i++) {
		for (int j = 0; j < result_column_num; j++) {
			outputfile << result[i][j] << "\t";
		}
		outputfile << "\n";
	}
	outputfile.close();
	/* ここまで */


	return 0;
}