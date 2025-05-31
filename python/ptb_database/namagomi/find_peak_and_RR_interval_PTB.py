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
 ・find_peak_and_RR_interval_PTB.py   ← 本ファイル
"""



import os
import csv
import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import find_peaks


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

step_num = 90000 # 90 [sec] 
                 # Ⅱ誘導の 90 [sec] のデータを抽出

extract_peak_num = 100  # ピークをこの数だけ取得する

poincare_plot_patient_no = 20   # 被験者番号を設定
                                # この番号の被験者のポアンカレプロットを表示する

# ユーザー入力 ################


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
    peaks, _ = find_peaks(y, distance = 650)
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


# delete the first row
peaks_arr = np.delete(peaks_arr, 0, axis=0)

# reshape the array to 4 x 3
peaks_arr = peaks_arr.reshape((row_size, column_size))

output_np_array_to_csv(peaks_arr, 'peaks_arr')

diff_peaks_arr = calculate_difference(peaks_arr)
output_np_array_to_csv(diff_peaks_arr, 'diff_peaks_arr')





# Extract the first row of A
row = diff_peaks_arr[poincare_plot_patient_no]
row_i = np.append(row, 0)
row_i_plus_1 = np.append(0, row)


fig, ax = plt.subplots()
ax.scatter(row_i, row_i_plus_1, marker="x")

# 直線 y = x 
x = np.linspace(0,10,100)
y = np.linspace(0,10,100)
ax.plot(x, y, color='black', alpha = 0.8, linestyle = "solid", linewidth = 0.8)
ax.tick_params(axis='x', which='major', labelsize = 12)
ax.tick_params(axis='y', which='major', labelsize = 12)

# グラフの範囲とラベル
plt.xlim(0.6, 1.2)
plt.ylim(0.6, 1.2)
plt.xlabel('RR interval (i)', fontsize = 12)
plt.ylabel('RR interval (i+1)', fontsize = 12)
plt.title('Poincaré plot - PTB', fontsize = 14)

plt.show()
