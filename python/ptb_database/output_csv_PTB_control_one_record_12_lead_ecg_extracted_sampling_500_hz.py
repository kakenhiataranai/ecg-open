"""
PTB Diagnostic ECG Database
12 + 3 lead ECG Recordings from 290 patients with various heart conditions

https://physionet.org/content/ptbdb/1.0.0/
https://www.kaggle.com/datasets/openmark/ptb-diagnostic-ecg-database

このスクリプト output_csv_PTB_control_one_record_12_lead_ecg_extracted_sampling_500_hz.py は

上のリンクのデータから健康対照群(healthy control)の指定する患者の12誘導ECGデータを抽出し

データは 0.001[sec]毎 なので偶数のインデックスのデータのみ残すことで 0.002[sec]毎 に間引いて
.csv に出力するスクリプト

です

被験者番号を選択することができます
ECG生データがら抽出する開始ステップと終了ステップを指定することができます
グラフ表示する誘導を選択することができます


【使用法】
 以下のファイルを同じフォルダに格納してください
  ・data_raw.npz
  ・meta_healthy_control.csv
  ・output_csv_PTB_control_one_record_12_lead_ecg_extracted_sampling_500_hz.py   ← 本ファイル

 実行すると halved_ecg_arr.csv を出力します
"""


import os
import csv
import numpy as np
import matplotlib.pyplot as plt

# ユーザー入力 ################
healthy_patient_number = 7  # 被験者番号
                            # 0, 1, 2, ... ,79 を入力
                            # meta_helthy_control.csv の データに対応                             
                            # データは重複あり 例えば 2 , 3 はどちらも patient117
                            # レコードと被験者番号の対応については record_id_and_no_correspondence_table.xlsx を参照

#start_step = 5000           # 表示するデータの開示時点 [step]
start_step = 5350
                            # 0, 1, 2, ... , 概ね2000まで を入力
                            # データサイズは 97000 ,115200, 118184, 120012 ステップのいづれか

#step_num = 2000             # 各誘導の 何 [sec] のデータを抽出するかを指定
step_num=600

                            # データのステップサイズは0.001[sec]なので step_num=10000で10[sec]分のデータ

lead_channel = 1            # グラフ表示する誘導を指定
                            # 0:Ⅰ誘導  1:Ⅱ誘導 2:Ⅲ誘導 # 3:aVR  4:aVL 5:aVF
                            # 6:V1 7:V2 8:V3 9:V4 10:V5 11:V6

##############################

ecg_num = 12                # 12誘導なので

##############################

# カレントディレクトリをスクリプトのディレクトリに設定
os.chdir(os.path.dirname(os.path.abspath(__file__)))
cwd = os.getcwd()
print(cwd)

# 生データ .npz 読み込み
npz = np.load('data_raw.npz')
print('number of patients =',len(npz))

# .csv をリスト形式で読み込み
csv_file = open("meta_healthy_control.csv", "r", encoding="ms932")
f = csv.reader(csv_file, delimiter=",", doublequote=True, lineterminator="\r\n", quotechar='"', skipinitialspace=True)
header = next(f)
print(header)
healthy_control_cnt = 0
for row in f:
    if healthy_control_cnt == healthy_patient_number:
        patient_data = row[0] +  "/" + row[1]
        break
    healthy_control_cnt += 1

csv_file.close()

print(healthy_control_cnt)



A = npz[patient_data]
data_size =  A.shape[0]
data_length = A.shape[0]/60/60
print('data size =',data_size,'step')
print('data length =',data_length,'min')

print(A.shape[1])


# 生データ .npz の サンプリングは1000[Hz]
step_size = 1000

# 生データ .npz のステップ数の半分
halved_step_num = int(step_num/2)


print('-----------------------------------')
twelve_ecg_arr = A[start_step : start_step + step_num, 0:ecg_num]
print('size of twelve_ecg_arr = ', twelve_ecg_arr.shape)


# 奇数のインデックスを削除しサンプリング0.002[sec]のデータに変換
even_indices = np.arange(0, step_num, 2)
halved_ecg_arr = np.random.random(( halved_step_num, ecg_num))
for i in range(ecg_num):
    halved_ecg_arr[:,i] = twelve_ecg_arr[even_indices,i]
print('size of halved_ecg_arr = ', halved_ecg_arr.shape)
print('-----------------------------------')


# 横軸 t を作成
t = np.arange(step_num)
t = t + start_step
t = t / step_size
t_halved = np.arange(step_num/2)
t_halved = t_halved/(step_size/2)
t_halved = t_halved + start_step/step_size


# グラフ作成
fig, ax = plt.subplots()
ax.plot(t, twelve_ecg_arr[:, lead_channel], color = 'navy', linestyle = "solid", linewidth = 1, label = "raw data")
plt.legend()
plt.show()



# .csv に出力
csv_filename = "halved_ecg_arr.csv"
with open(csv_filename, mode='w', newline='') as file:
    writer = csv.writer(file)
    writer.writerows(halved_ecg_arr)

