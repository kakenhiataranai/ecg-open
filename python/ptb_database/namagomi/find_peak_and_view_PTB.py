"""
PTB Diagnostic ECG Database
12 + 3 lead ECG Recordings from 290 patients with various heart conditions

https://physionet.org/content/ptbdb/1.0.0/
https://www.kaggle.com/datasets/openmark/ptb-diagnostic-ecg-database

このスクリプト find_peak_PTB.py は

上のリンクのデータから健康対照群(healthy control)の患者のECGデータを抽出し
その時系列のQRS波のピークを抽出しその様子を閲覧するスクリプト

です

【使用法】
 以下のファイルを同じフォルダに格納してください
  ・data_raw.npz
  ・meta_healthy_control.csv
  ・find_peak_and_view_PTB.py   ← 本ファイル
"""


import os
import csv
import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import find_peaks


# ユーザー入力 ################
healthy_patient_number = 7  # 被験者番号
                            # 0, 1, 2, ... ,79 を入力
                            # meta_helthy_control.csv の データに対応                             
                            # データは重複あり 例えば 2 , 3 はどちらも patient117 

start_step = 0              # 表示するデータの開示時点 [step]
                            # 0, 1, 2, ... , 概ね2000まで を入力
                            # データサイズは 97000 ,115200, 118184, 120012 ステップのいづれか

step_num = 5800             # 各誘導の 何 [sec] のデータを抽出するかを指定
                            # データのステップサイズは0.001[sec]なので step_num=10000で10[sec]分のデータ

lead_channel = 1           # どの誘導かを指定
                            # 0:Ⅰ誘導  1:Ⅱ誘導 2:Ⅲ誘導 # 3:aVR  4:aVL 5:aVF
                            # 6:V1 7:V2 8:V3 9:V4 10:V5 11:V6

##############################


# カレントディレクトリをスクリプトのディレクトリに設定
os.chdir(os.path.dirname(os.path.abspath(__file__)))
cwd = os.getcwd()
print(cwd)

# 生データ .npz 読み込み
npz = np.load('data_raw.npz')
print('number of patients =',len(npz))
# print(npz.files)

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




# サンプリングは1000[Hz]
step_size = 1000



graph_step_num = 2 * step_num

# Ⅱ誘導を抽出
ecg = A[start_step : start_step + step_num, lead_channel]

# Ⅱ誘導のピークを求める
peaks, _ = find_peaks(ecg, distance = 550)


# 横軸 t を作成
t = np.arange(step_num)
t = t + start_step
t = t / step_size

# グラフ作成
fig, ax = plt.subplots()
ax.plot(t, ecg, color = 'navy', linestyle = "solid", linewidth = 1)
ax.plot((start_step + peaks) / step_size, ecg[peaks], color = 'orange', linestyle = "none", marker = 'x')
plt.show()

print(peaks)
