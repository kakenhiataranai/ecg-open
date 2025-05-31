import os
import json
import matplotlib.pyplot as plt
import numpy as np


def set_current_directory():
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    cwd = os.getcwd()
    print(cwd)

set_current_directory()


# pseudo_ecg.jsonのファイルパス
file_path = "pseudo_ecg.json"

# ファイル読み込み
with open(file_path) as f:
    # 複数のJSONオブジェクトをリストとして読み込む
    data_list = [json.loads(line) for line in f]


# グラフの行数と列数を指定
row_num = 6
col_num = 2

# サブプロットを作成
fig, axs = plt.subplots(row_num, col_num, figsize=(10, 12))

# 表示するリードの順番
leads_order = ['i', 'ii', 'iii', 'avr', 'avl', 'avf', 'v1', 'v2', 'v3', 'v4', 'v5', 'v6']

# プロット
for i, lead in enumerate(leads_order):
    row = i % row_num
    if i < row_num:
        col = 0
    else:
        col = 1
    lead_values = [entry['ecg'][lead] for entry in data_list]
    axs[row, col].plot(lead_values)
    axs[row, col].set_title(lead)
    axs[row, col].set_xlabel('Data Point')
    axs[row, col].set_ylabel('ECG')


plt.subplots_adjust(left=0.05, right=0.98, bottom=0.05, top=0.95)
plt.show()