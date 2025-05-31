
"""
このスクリプト display_ecg_from_generator.py は

C++で生成した擬似的な12誘導心電図の.csvを作成したフォーマットで閲覧するスクリプト

です

【使用法】
以下のファイルを同じフォルダに格納してください
 ・pseudo_ecg_result.csv
 ・display_ecg_from_generator.py   ← 本ファイル

"""


import os
import csv
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as tick



# ユーザー入力 ################

#start_step = 800             # 表示するデータの開示時点 [step]
#start_step = 2500             # 表示するデータの開示時点 [step] 
start_step = 2200             # 表示するデータの開示時点 [step]  

                             # 0, 1, 2, ... , 概ね2000まで を入力                             

load_csv_file_name = 'pseudo_ecg.csv'   # pseudo_ecg.csv のデータサイズは C++側で実行した結果である .json のオブジェクト数

##############################


# カレントディレクトリをスクリプトのディレクトリに設定
def set_current_directory():
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    cwd = os.getcwd()
    print(cwd)
    return cwd


cwd = set_current_directory()

# Define the path to the CSV file
file_path = cwd + '\\' + load_csv_file_name 
csv_data = np.genfromtxt(file_path, delimiter=',')

A = csv_data

data_size =  A.shape[0]
data_length = A.shape[0]/60/60
print('data size =',data_size,'step')
print('data length =',data_length,'min')
print(A.shape[1])

# 各誘導の 5 [sec] のデータを抽出
step_num = 2500
graph_step_num = 2 * step_num

# 12誘導
ecg_channel_num = 12

# 横軸 t を作成
t_array = np.arange(0, graph_step_num)
t_array = t_array / 500

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
    if i < ecg_channel_num/2:
        color_input = 'black'
    else:
        #color_input = 'blue'
        color_input = 'black'
    ax.plot(t_array, y_tmp, color = color_input, linestyle = "solid", linewidth = 0.9)

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
