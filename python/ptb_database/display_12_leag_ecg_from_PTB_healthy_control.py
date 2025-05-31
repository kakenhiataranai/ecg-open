
"""
PTB Diagnostic ECG Database
12 + 3 lead ECG Recordings from 290 patients with various heart conditions

https://physionet.org/content/ptbdb/1.0.0/
https://www.kaggle.com/datasets/openmark/ptb-diagnostic-ecg-database

このスクリプト display_12_leag_ecg_from_PTB_healthy_control.py は
上のリンクのダウンロードデータから健康対照群(healthy control)の患者のECGデータを抽出し閲覧するスクリプト
です

【使用法】
以下のファイルを同じフォルダに格納してください
 ・data_raw.npz
 ・meta_healthy_control.csv
 ・display_12_leag_ecg_from_PTB_healthy_control.py   ← 本ファイル

"""


import os
import csv
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as tick


# ユーザー入力 ################

healthy_patient_number = 7  # 被験者番号
                             # 0, 1, 2, ... ,79 を入力
                             # meta_helthy_control.csv の データに対応                             
                             # データは重複あり 例えば 2 , 3 はどちらも patient117 

start_step = 800             # 表示するデータの開示時点 [step]
                             # 0, 1, 2, ... , 概ね2000まで を入力
                             # データサイズは 97000 ,115200, 118184, 12012 ステップのいづれか


##############################


# カレントディレクトリをスクリプトのディレクトリに設定
def set_current_directory():
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    cwd = os.getcwd()
    print(cwd)

# 生データ .npz 読み込み
def load_data_row_npz():
    npz = np.load('data_raw.npz')
    patient_num = len(npz)
    print('number of patients =', patient_num)
    return npz



set_current_directory()
npz  = load_data_row_npz()



# .csv をリスト形式で読み込み
# healthy_patient_number で指定した被験者を抽出するためのタグを作成
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

# 各誘導の 5 [sec] のデータを抽出
step_num = 5000
graph_step_num = 2 * step_num

# 12誘導
ecg_channel_num = 12

# 横軸 t を作成
t_array = np.arange(0, graph_step_num)
t_array = t_array / 1000

# 50/5 = 10 [sec]
x_axis_array = np.arange(0, 50) 
x_axis_array = x_axis_array / 5
y_axis_array = np.arange(0, 31) 
y_axis_array = y_axis_array / 2
y_axis_array = y_axis_array - 13.5

y_tmp_add = np.ones(step_num) * 10000  # 十分大きい値

fig, ax = plt.subplots()
for i in range(ecg_channel_num):
    ecg_index = 0
    y_tmp = A[start_step : start_step + step_num, i]

    if i < ecg_channel_num / 2:
        y_tmp = np.concatenate([y_tmp, y_tmp_add], 0)
        y_tmp = y_tmp - 2.5 * i
    else:
        y_tmp = np.concatenate([y_tmp_add, y_tmp], 0)
        y_tmp = y_tmp - 2.5 * (i - ecg_channel_num / 2)
   
    y_tmp = np.ma.masked_where(y_tmp >= 1000, y_tmp)

   # if i != 8:
    ax.plot(t_array, y_tmp, color = 'black', linestyle = "solid", linewidth = 0.9)

ax.set_xticks(x_axis_array)
ax.set_yticks(y_axis_array)
ax.grid(which = "major", axis = "x", color = "darkorange", alpha = 1, linestyle = "solid", linewidth = 1.0)
ax.grid(which = "major", axis = "y", color = "darkorange", alpha = 1, linestyle = "solid", linewidth = 1.0)

ax.grid(which = "minor", axis = "x", color = "darkorange", alpha = 1.0, linestyle = "solid", linewidth = 0.4)
ax.grid(which = "minor", axis = "y", color = "darkorange", alpha = 0.8, linestyle = "solid", linewidth = 0.4)

ax.xaxis.set_minor_locator(tick.MultipleLocator(0.04))
ax.yaxis.set_minor_locator(tick.MultipleLocator(0.1))

ax.axes.xaxis.set_ticklabels([])
ax.axes.yaxis.set_ticklabels([])



plt.subplots_adjust(left=0.05, right=0.95, bottom=0.05, top=0.95)
plt.xlim(0, 10)
plt.ylim(-13.5, 1.5)


plt.show()
