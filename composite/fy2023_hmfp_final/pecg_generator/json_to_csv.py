
"""
このスクリプト json_to_csv.py は

C++で生成した擬似的な12誘導心電図の .jsonを .csv に変換するスクリプト

です

【使用法】
以下のファイルを同じフォルダに格納してください
 ・pseudo_ecg.json
 ・json_to_csv.py   ← 本ファイル

実行すると以下のファイルを同じフォルダに出力します
 ・pseudo_ecg.csv

"""


import os
import csv
import json


def set_current_directory():
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    cwd = os.getcwd()
    print(cwd)

set_current_directory()

# jsonファイルのパス
json_file_path = "pseudo_ecg.json"

# csvファイルのパス
csv_file_path = "pseudo_ecg.csv"

# jsonファイルを読み込む
with open(json_file_path) as json_file:
    data_list = [json.loads(line) for line in json_file]

# csvファイルに書き込む
with open(csv_file_path, 'w', newline='') as csv_file:
    # csvライターを作成
    csv_writer = csv.writer(csv_file)

    # データを書き込む
    for entry in data_list:
        row_data = [entry['pecg'][lead] for lead in ['i', 'ii', 'iii', 'avr', 'avl', 'avf', 'v1', 'v2', 'v3', 'v4', 'v5', 'v6']]
        csv_writer.writerow(row_data)

print(f"csvファイル {csv_file_path} が作成されました。")
