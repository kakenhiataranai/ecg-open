#!/bin/bash

# 20回繰り返す
for ((i=0; i<20; i++))
do
    # ./outを実行
    ./out

    # ファイル名を検索してリネーム
    filename="result_ecg_case_3_${i}.csv"
    if [ -e "result_ecg_case_3_.csv" ]; then
        mv "result_ecg_case_3_.csv" "$filename"
        echo "Renamed result_ecg_case_3_.csv to $filename"
    else
        echo "Error: result_ecg_case_3_.csv not found."
    fi
done
