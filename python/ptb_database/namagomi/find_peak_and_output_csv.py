"""
PTB Diagnostic ECG Database
12 + 3 lead ECG Recordings from 290 patients with various heart conditions

https://physionet.org/content/ptbdb/1.0.0/
https://www.kaggle.com/datasets/openmark/ptb-diagnostic-ecg-database

このスクリプト find_peak_and_RR_interval_and_autocorrelation_PTB.py は

 -上のリンクのダウンロードデータから健康対照群(healthy control)の患者のECGデータを抽出
 -その時系列のQRS波のピークを抽出
 -すべての被験者のピーク時間[sec] の peaks_arr.csv を出力
 -すべての被験者の RR-interval[sec] の diff_peaks_arr.csv を出力

です


【使用法】
 以下のファイルを同じフォルダに格納してください
 ・data_raw.npz
 ・meta_healthy_control.csv
 ・find_peak_and_output_csv.py   ← 本ファイル
"""





import os
import csv
import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import find_peaks
import statsmodels.api as sm

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


# .csv をリスト形式で読み込み
def read_meta_healthy_control_csv():
    csv_file = open("meta_healthy_control.csv", "r", encoding="ms932")
    f = csv.reader(csv_file, delimiter=",", doublequote=True, lineterminator="\r\n", quotechar='"', skipinitialspace=True)
    header = next(f)
    healthy_control_patient_record_id_list = []
    for row in f:
        patient_and_record_id = row[0] +  "/" + row[1]
        healthy_control_patient_record_id_list.append(patient_and_record_id)
    csv_file.close()
    return healthy_control_patient_record_id_list


# np.array を .csv に出力
def output_np_array_to_csv(np_array, csv_name):

    # Open a CSV file for writing
    with open(csv_name + '.csv', 'w', newline='') as file:
        writer = csv.writer(file)

        # Write the data to the CSV file
        for row in np_array:
            writer.writerow(row)


def calculate_difference(A):
    # Get the shape of the input array A
    m, n = A.shape
    
    # Initialize a new array B of shape (m, n-1) with zeros
    B = np.zeros((m, n-1))
    
    # Calculate the differences for each element in B
    for i in range(m):
        for j in range(n-1):
            B[i][j] = A[i][j+1] - A[i][j]
            
    return B


# ユーザー入力 ################

step_num = 150000       # 150 [sec]                  
extract_peak_num = 170  # ピークをこの数だけ取得する


##############################




set_current_directory()
npz = load_data_row_npz()
healthy_control_patient_record_id_list = read_meta_healthy_control_csv()

empty_arr = np.empty(0)
peaks_init = np.zeros(extract_peak_num)
peaks_arr = np.append(empty_arr, peaks_init)

for i in range(len(healthy_control_patient_record_id_list)):
    A =  npz[healthy_control_patient_record_id_list[i]]
    data_size =  A.shape[0]
    data_length = A.shape[0] / 60 / 60
    y = A[0 : step_num, 1]   # Ⅱ誘導を抽出
    peaks, _ = find_peaks(y, distance = 550)
    peak_num = len(peaks)
    if peak_num > extract_peak_num:
        peaks = peaks[:extract_peak_num]
    elif peak_num < extract_peak_num:
        for i in range(extract_peak_num - peak_num):
            peaks = np.append(peaks, 0) 
    peaks_arr = np.vstack((peaks_arr, peaks))



peaks_arr = peaks_arr / 1000 

print(peaks_arr)


row_size = peaks_arr.shape[0] - 1
column_size = peaks_arr.shape[1]

peaks_arr = np.delete(peaks_arr, 0, axis = 0)
peaks_arr = peaks_arr.reshape((row_size, column_size))

output_np_array_to_csv(peaks_arr, 'peaks_arr')

diff_peaks_arr = calculate_difference(peaks_arr)
output_np_array_to_csv(diff_peaks_arr, 'diff_peaks_arr')

print('finished')