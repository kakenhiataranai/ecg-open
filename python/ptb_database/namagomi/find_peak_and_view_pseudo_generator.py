"""
このスクリプト find_peak_and_view_pseudo_generator.py は

C++で生成した擬似的な12誘導心電図の.csvを読み込んでⅡ誘導のピークを検出し閲覧するスクリプト

です

【使用法】
 以下のファイルを同じフォルダに格納してください
  ・pseudo_ecg_result_tablex_x_patient.csv.csv
  ・find_peak_and_view_pseudo_generator.py   ← 本ファイル
"""


import os
import csv
import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import find_peaks


# ユーザー入力 ################

load_csv_file_name = 'pseudo_ecg_result_table1_1_patient.csv'     # 読み込む.csvファイル名
#load_csv_file_name = 'pseudo_ecg_result_table3_13_patients.csv'

start_step = 2500            # 表示するデータの開始時点 [step]
                             # 2500 * 0.002 = 5 [sec] にしておく

step_num = 5000             # 各誘導の 何 [sec] のデータを抽出するかを指定
                             # データのステップサイズは0.002[sec]なので step_num = 5000で10[sec]分のデータ
##############################


# カレントディレクトリをスクリプトのディレクトリに設定
def set_current_directory():
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    cwd = os.getcwd()
    print(cwd)
    return cwd

# C++の生成器で出力した.csvを読み込む
def load_generator_csv():
    # Define the path to the CSV file
    file_path = cwd + '\\' + load_csv_file_name 
    csv_data = np.genfromtxt(file_path, delimiter=',')
    return csv_data


cwd = set_current_directory()
A = load_generator_csv()

data_size =  A.shape[0]
data_length = A.shape[0]/60/60
print('data size =',data_size,'step')
print('data length =',data_length,'min')
print(A.shape[1])




# サンプリングは500[Hz]
step_size = 500

graph_step_num = 2 * step_num

# Ⅱ誘導を抽出
ecg = A[start_step : start_step + step_num, 1]

# Ⅱ誘導のピークを求める
peaks, _ = find_peaks(ecg, distance = 250)  # 250 * 0.002 = 0.5 [sec]


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
