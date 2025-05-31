
"""
execute.sh を繰り返し実行するスクリプト

フィッティング結果のグラフを出力する
pecgnew.conf の gnuplot=1; としておくこと
is_optimize の True / False を設定することで実行する lead を設定する

"""

# coding: utf-8
import os
import sys
import config_dataclass as cdc
import script_executor as se
import file_manipulation_lib as fmlib

# 設定ファイル名
CONFIG_FILENAME = 'pecgnew_config.ini'



def format_number(n: int) -> str:
    if 0 <= n <= 9:
        # 一桁の整数の場合、"0n"形式にする
        return f'{n:02d}'
    elif 10 <= n <= 99:
        # 二桁の整数の場合、そのまま文字列として返す
        return str(n)
    else:
        raise ValueError("The integer must be between 0 and 99, inclusive.")


def main():

    # 設定ファイル読み込み
    config_ini_path = CONFIG_FILENAME    
    auto_exe_setting = cdc.read_config(config_ini_path)


    # Ⅲ, aVR, aVL, aVF は最適化しない
    #is_optimize = [True, True, False, False, False, False, True, True, True, True, True, True]
    is_optimize = [True, True, True, True, True, True, True, True, True, True, True, True]
    lead_index = 0

    # 最適化くりかえし
    for lead in is_optimize:
        if(not(is_optimize)):
            lead_index += 1
            break
        
        # pecgnew.conf のlead を変更
        fmlib.replace_value_in_config(auto_exe_setting.dir, 
                                  auto_exe_setting.config_filename, 
                                  auto_exe_setting.lead_key, 
                                  lead_index)

        # pecgnew を実行
        se.execute_bash_script_or_shell_script(auto_exe_setting.dir, auto_exe_setting.exec_filename)

        # 出力されるグラフ(pecgnew_plot.png)をリネーム
        rename_filename = auto_exe_setting.plot_rename_filename_wo_extension + format_number(lead_index) + auto_exe_setting.plot_rename_filename_extention
        fmlib.rename_file(auto_exe_setting.dir, auto_exe_setting.plot_original_filename, rename_filename)

        lead_index += 1

    # lead を元の値に書き換える


if __name__ == "__main__":
    main()


    