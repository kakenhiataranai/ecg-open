
"""
このスクリプト view_comparison_of_two_12_lead_ecgs.py は

https://github.com/seetheworld1992/electrocardiogram/tree/main/src/c%2B%2B/pseudo_ecg_generation/modelling_study/20230914_function_selection_and_parameter_spaces

の出力ファイルである

ratio_0.xx_record_no_xx_r_peak_no_xx.csv
ratio_0.xx_record_no_xx_r_peak_no_xx_optimized.csv'

をグラフ化して比較検討するためのスクリプト
です

フォーマットが同じであれば，それ以外の目的にも使用することができます


【使用法】
読み込みたい２つの.csvファイルを本ファイルを格納しているフォルダに格納してください

"""

# ユーザー入力 ################################################################

# 2つの.csvファイルのパスを指定
csv_file1 = 'ratio_0.35_record_no_0_r_peak_no_1.csv'
csv_file2 = 'ratio_0.35_record_no_0_r_peak_no_1_optimized.csv'

##############################################################################



import os
import matplotlib.pyplot as plt
import numpy as np 
import pandas as pd

# カレントディレクトリをスクリプトのディレクトリに設定
def set_current_directory():
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    cwd = os.getcwd()
    print(cwd)
    return cwd


cwd = set_current_directory()

# データを読み込む
data1 = pd.read_csv(csv_file1, header=None)
data2 = pd.read_csv(csv_file2, header=None)

# 各要素から同じ列の0行目の値をマイナス
data1 = data1 - data1.iloc[0]

# グラフの行数と列数を指定
row_num = 6
col_num = 2

# ステップサイズ
step_size = 0.002

# 横軸 （いまはうまくいってないので未使用）
x_array_size = data1.shape[0]
x_axis_array = np.arange(0, x_array_size) 
x_axis_array = x_axis_array * 0.002

# グラフのサイズを指定
fig, ax = plt.subplots(row_num, col_num)

# グラフタイトルと縦位置調整
fig.suptitle('comparison of ECG raw data and fitted', y = 0.99)

# グラフを描画
for col in range(col_num):
    for row in range(row_num):        
        ax[row, col].plot(x_axis_array, data1.iloc[:, row + col*row_num], marker='None', color = 'blue', linestyle = 'solid', label = 'raw data')
        ax[row, col].plot(x_axis_array, data2.iloc[:, row + col*row_num], marker='None', color = 'magenta', linestyle = 'solid', label = 'fitted')
        ax[row, col].legend()
        ax[row, col].grid()

# 余白の調整
plt.subplots_adjust(left=0.05, right=0.98, bottom=0.05, top=0.95)

# グラフを表示
plt.show()