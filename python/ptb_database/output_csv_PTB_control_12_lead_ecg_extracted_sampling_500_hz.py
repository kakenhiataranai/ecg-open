"""
---------------------------------------------------------------------------------
PTB Diagnostic ECG Database
12 + 3 lead ECG Recordings from 290 patients with various heart conditions

https://physionet.org/content/ptbdb/1.0.0/
https://www.kaggle.com/datasets/openmark/ptb-diagnostic-ecg-database
---------------------------------------------------------------------------------


このスクリプト output_csv_PTB_control_12_lead_ecg_extracted_sampling_500_hz.py は

上のリンクのデータから健康対照群(healthy control)の患者80レコード(重複ありなので80人ではない)
の12誘導ECGデータを抽出し
データは 0.001[sec]毎 なので偶数のインデックスのデータのみ残すことで 0.002[sec]毎 に間引いて
.csv に出力するスクリプト

出力する.csv の 
縦軸は0.002[sec]毎 control群のデータのなかで最も多い時間方向のデータ数 120012行
横軸は960(=12×80)列

meta_healthy_control.csvのレコード2からレコード81までのECGデータを列方向に順に
Ⅰ,Ⅱ,Ⅲ,aVR,aVL,aVF,V1,V2,V3,V4,V5,V6,Ⅰ,Ⅱ,Ⅲ,....
のように並べる


【使用法】
 以下のファイルを同じフォルダに格納してください
  ・data_raw.npz
  ・meta_healthy_control.csv
  ・output_csv_PTB_control_12_lead_ecg_extracted_sampling_500_hz.py   ← 本ファイル

   実行すると PTB_control_12_lead_ecg_extracted_sampling_500_hz.csv を出力する
"""


import os
import csv
import numpy as np

ecg_num = 12                # 12誘導なので
control_num = 80            # control群 は80レコード
max_sig_len = 120012        # control群のデータのなかで最も多い時間方向のデータ数
                            # 120012 /1000 なので概ね 12[sec]
step_size = 1000            # 生データ .npz の サンプリングは1000[Hz]

# カレントディレクトリをスクリプトのディレクトリに設定
os.chdir(os.path.dirname(os.path.abspath(__file__)))
cwd = os.getcwd()
print(cwd)

# 生データ .npz 読み込み
npz = np.load('data_raw.npz')
print('number of patients in data_raw.npz =',len(npz))

# meta_healthy_control.csvのレコード毎にA列とB列から.npzへの入力を作成
output_csv_col_idx=0
for i in range(1, 1 + control_num):

    # meta_healthy_control.csv をリスト形式で読み込み
    csv_file = open("meta_healthy_control.csv", "r", encoding="ms932")
    f = csv.reader(csv_file, delimiter=",", doublequote=True, lineterminator="\r\n", quotechar='"', skipinitialspace=True)
    header = next(f)
    #print(header)
    healthy_control_cnt = 0
    for row in f:
        if healthy_control_cnt == i:
            patient_data = row[0] +  "/" + row[1]
            break
        healthy_control_cnt += 1

    csv_file.close()

    # レコード i のECGデータを抽出
    A = npz[patient_data]

    # 15誘導あるので12誘導のみ抽出
    A =A[:,0:ecg_num]

    if A.shape[0] < max_sig_len:
        new_arr = np.zeros((max_sig_len, ecg_num))
        new_arr[:A.shape[0], :] = A
        A = new_arr

    # 初回はarrを作成
    if i == 1: 
        arr = A
    else:
        arr = np.concatenate((arr, A), axis=1)

    output_csv_col_idx += ecg_num


# 生データ .npz のステップ数の半分
halved_step_num = int(max_sig_len/2)

# 奇数のインデックスを削除しサンプリング0.002[sec]のデータに変換
even_indices = np.arange(0, max_sig_len, 2)
halved_arr = np.zeros(( halved_step_num, output_csv_col_idx))
for i in range(output_csv_col_idx):
    halved_arr[:,i] = arr[even_indices,i]

# halved_arr を .csv に出力
csv_filename = "PTB_control_12_lead_ecg_extracted_sampling_500_hz.csv"
with open(csv_filename, mode='w', newline='') as file:
    writer = csv.writer(file)
    writer.writerows(halved_arr)

print('finished')
