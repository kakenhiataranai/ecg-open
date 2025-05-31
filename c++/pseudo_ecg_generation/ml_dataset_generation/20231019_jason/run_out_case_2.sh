#!/bin/bash

# 20回繰り返す
for ((i=0; i<20; i++))
do
    # ./outを実行
    ./out

    # ファイル名を検索してリネーム
    filename="result_ecg_case_2_${i}.csv"
    if [ -e "result_ecg_case_2_.csv" ]; then
        mv "result_ecg_case_2_.csv" "$filename"
        echo "Renamed result_ecg_case_2_.csv to $filename"
    else
        echo "Error: result_ecg_case_2_.csv not found."
    fi
done
