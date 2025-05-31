// generate_ecg_BVAM_main.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>
#include <fstream>

#include "ecg_BVAM_model.h"

int main()
{
	/* 計算パラメータ作成 ここから */
	double step_size = 0.005;   /* [sec] */
	int step_num = 2000;        /* シミュレーションステップ数  */
	int variable_num = 4;
	std::vector<double> alpha(variable_num, 0);
	std::vector<double> ecg_result(step_num, 0);
	std::vector<std::vector<double>> x_result(step_num, std::vector<double>(variable_num));  /* サイズは (step_num × 4) */


	/* test */
	double large_c = 1.35;
	double large_h = 2.74;
	double beta = 4;
	double gamma_t = 1;
	alpha[0] = 0;
	alpha[1] = 0;
	alpha[2] = 0;
	alpha[3] = 0;


	///* normal sinus rhythm */
	//double large_c = 1.35;
	////double large_h = 3;
	////double large_h = 2.729;
	//double large_h = 2.164;
	//double beta = 4;
	//double gamma_t = 7;
	//alpha[0] = -0.024;
	//alpha[1] = 0.0216;
	//alpha[2] = -0.0012;
	//alpha[3] = 0.12;

	///* Sinus Tachycardia */
	//double large_c = 1.35;
	//double large_h = 2.848;
	//double beta = 4;
	//double gamma_t = 21;	
	//alpha[0] = 0;
	//alpha[1] = -0.1;
	//alpha[2] = 0;
	//alpha[3] = 0;

	///* Atrial Flutter */
	//double large_c = 1.35;
	//double large_h = 1.52;
	//double beta = 4;
	//double gamma_t = 13;
	//alpha[0] = -0.068;
	//alpha[1] = 0.028;
	//alpha[2] = -0.024;
	//alpha[3] = 0.12;

	///* Ventricular Tachycardia */
	//double large_c = 1.35;
	//double large_h = 2.178;
	//double beta = 4;
	//double gamma_t = 21;
	//alpha[0] = 0;
	//alpha[1] = 0;
	//alpha[2] = 0;
	//alpha[3] = -0.1;

	///* Ventricular Flutter */
	//double large_c = 1.35;
	//double large_h = 2.178;
	//double beta = 4;
	//double gamma_t = 13;
	//alpha[0] = 0.1;
	//alpha[1] = -0.02;
	//alpha[2] = -0.01;
	//alpha[3] = 0;

	
	/* ここまで */


	/* 心電図の生成 計算実行 ここから */
	ecg_signals ecg_obj(step_size, step_num, large_c, large_h, beta, gamma_t, alpha);
	ecg_obj.generate_ecg_signal_one_cycle(ecg_result, x_result);	
	/* ここまで */


	/* テキストファイルに書き込みして出力 ここから */
	std::string filename = "ecg_result.txt";
	std::ofstream outputfile(filename);
	for (int i = 0; i < step_num; i++) {
			outputfile << ecg_result[i] << "\n";
	}
	outputfile.close();


	std::string filename1 = "x_result.txt";  /* x_1, x_2, x_3, x_4  */
	std::ofstream outputfile1(filename1);
	for (int i = 0; i < step_num; i++) {
		for (int j = 0; j < variable_num; j++) {
			outputfile1 << x_result[i][j] << "\t";
		}
		outputfile1 << "\n";
	}
	outputfile1.close();
	/* ここまで */


}

// プログラムの実行: Ctrl + F5 または [デバッグ] > [デバッグなしで開始] メニュー
// プログラムのデバッグ: F5 または [デバッグ] > [デバッグの開始] メニュー

// 作業を開始するためのヒント: 
//    1. ソリューション エクスプローラー ウィンドウを使用してファイルを追加/管理します 
//   2. チーム エクスプローラー ウィンドウを使用してソース管理に接続します
//   3. 出力ウィンドウを使用して、ビルド出力とその他のメッセージを表示します
//   4. エラー一覧ウィンドウを使用してエラーを表示します
//   5. [プロジェクト] > [新しい項目の追加] と移動して新しいコード ファイルを作成するか、[プロジェクト] > [既存の項目の追加] と移動して既存のコード ファイルをプロジェクトに追加します
//   6. 後ほどこのプロジェクトを再び開く場合、[ファイル] > [開く] > [プロジェクト] と移動して .sln ファイルを選択します
