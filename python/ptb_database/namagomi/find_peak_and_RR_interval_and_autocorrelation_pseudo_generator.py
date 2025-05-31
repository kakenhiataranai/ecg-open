"""
このスクリプト find_peak_and_view_pseudo_generator.py は

C++で生成した擬似的な12誘導心電図の.csvを読み込んで
 - Ⅱ誘導のピークを検出しRRを算出
 - RR interval を.csvで出力
 - RR interval のポアンカレプロットを閲覧
 
 するスクリプトです

【使用法】
 以下のファイルを同じフォルダに格納してください
  ・pseudo_ecg_result_tablex_x_patient.csv.csv
  ・find_peak_and_RR_interval_pseudo_generator.py   ← 本ファイル
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
    return cwd


# C++の生成器で出力した.csvを読み込む
def load_generator_csv():
    # Define the path to the CSV file
    file_path = cwd + '\\' + load_csv_file_name 
    csv_data = np.genfromtxt(file_path, delimiter=',')
    return csv_data


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

load_csv_file_name = 'pseudo_ecg_result_table1_1_patient.csv'
#load_csv_file_name = 'pseudo_ecg_result_table3_13_patients.csv'


start_step = 2500            # 使用するデータの開始時点 [step]
                             # 2500 * 0.002 = 5 [sec] にしておく

step_num = 45000 # 90 [sec] 
                 # Ⅱ誘導の 90 [sec] のデータを抽出

extract_peak_num = 100  # ピークをこの数だけ取得する

sampling_freq = 500

##############################

cwd = set_current_directory()
A = load_generator_csv()

empty_arr = np.empty(0)
peaks_init = np.zeros(extract_peak_num)
peaks_arr = np.append(empty_arr, peaks_init)

# data_size =  A.shape[0]
# data_length = A.shape[0] / 60 / 60
y = A[start_step : start_step + step_num, 1]   # Ⅱ誘導を抽出
peaks, _ = find_peaks(y, distance = 250)       # 250 * 0.002 = 0.5 [sec]
peak_num = len(peaks)
peaks_arr = np.vstack((peaks_arr, peaks))

peaks_arr = peaks_arr / sampling_freq 

print(peaks_arr)


row_size = peaks_arr.shape[0] - 1
column_size = peaks_arr.shape[1]


# delete the first row
peaks_arr = np.delete(peaks_arr, 0, axis=0)

# reshape the array
peaks_arr = peaks_arr.reshape((row_size, column_size))

output_np_array_to_csv(peaks_arr, 'peaks_arr_pseudo')

diff_peaks_arr = calculate_difference(peaks_arr)
output_np_array_to_csv(diff_peaks_arr, 'diff_peaks_arr_pseudo')


# Extract the first row of A
row = diff_peaks_arr
row_i = np.append(row, 0)
row_i_plus_1 = np.append(0, row)



row = np.delete(row, 0)
row = np.delete(row, -1)


sm.graphics.tsa.plot_acf(row, lags = 50)
plt.show()