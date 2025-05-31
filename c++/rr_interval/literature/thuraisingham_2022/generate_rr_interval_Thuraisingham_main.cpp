// generate_rr_interval_Thuraisingham_main.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>
#include <fstream>

#include "rr_interval_Thuraisingham_model.h"


int main()
{
	/* 計算パラメータ作成 ここから */
	double step_size = 0.002;             /* [sec] */
	int step_num_per_sec = 500;           /* = 1 / 0.002  */
	 //int step_num = 900000;                 /* シミュレーションステップ数   */
	int step_num = 900000;                 /* シミュレーションステップ数   900000 [step] * 0.002 [sec] / 60 [sec] = 30 [min]  */
	
	//double beat_frequency_in_bpm = 86.30; /* 論文に書いてあるM1(被験者1)の値  */
	//double beat_frequency_in_bpm = 60.17; /* 論文に書いてあるM2(被験者2)の値  */
	double beat_frequency_in_bpm = 75.92; /* 論文に書いてあるM3(被験者3)の値  */

	int large_g_t_term_in_sec = 10800;    /* インプルーブブドロジスティック写像が生成する列の期間 [秒]         */
	double large_c_initial = 0.1;         /* インプルーブブドロジスティック写像の初期値   [-1, 1] の範囲で設定 */
	double large_a = 19;                  /* インプルーブブドロジスティック写像の係数                           *
	/* ここまで */

	int sa_node_result_column_num = 3;
	std::vector<std::vector<double>> sa_node_action_potential_result(step_num, std::vector<double>(sa_node_result_column_num));  /* サイズは (step_num × 3) */
	std::vector<double> rr_interval_result;
		
	/* 洞房結節の活動電位の生成 計算実行 ここから */
	sinoatrial_node_action_potential sinoatrial_node_action_potential_obj(step_size, step_num, step_num_per_sec, beat_frequency_in_bpm, large_g_t_term_in_sec, large_c_initial, large_a);
	sinoatrial_node_action_potential_obj.generate_sinoatrial_node_action_potential_one_cycle(sa_node_action_potential_result, rr_interval_result);
	/* ここまで */


	/* テキストファイルに書き込みして出力 ここから */
	std::string filename = "sinoatrial_node_action_potential_result.txt";
	std::ofstream outputfile(filename);
	for (int i = 0; i < step_num; i++) {
		for (int j = 0; j < sa_node_result_column_num; j++) {
			outputfile << sa_node_action_potential_result[i][j] << "\t";
		}
		outputfile << "\n";
	}
	outputfile.close();


	int rr_interval_result_size = rr_interval_result.size();
	std::string filename1 = "rr_interval_result.txt";
	std::ofstream outputfile1(filename1);
	for (int i = 0; i < rr_interval_result_size; i++) {
		outputfile1 << rr_interval_result[i] << "\n";
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
