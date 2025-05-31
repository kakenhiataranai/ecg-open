# coding: utf-8
import os
import sys
import time
import config_dataclass as cdc
import initialization_routine as ir
import child_process_routine as cpr
import script_executor as se


# 設定ファイル名
CONFIG_FILENAME = 'pecg_config.ini'

def main():

    # 設定ファイル読み込み
    config_ini_path = CONFIG_FILENAME    
    pub_setting, gen_setting, sub_setting, dev_shm_setting, heart_json_setting = cdc.read_config(config_ini_path)

    # MQTT起動前のシステム初期設定の処理
    ir.initialization_routine(pub_setting, gen_setting, sub_setting, dev_shm_setting, heart_json_setting)

    # MQTT 開始
    try:    
        # バックグラウンドプロセス : 心拍データ MQTT Subscribe
        # C++ 実行ファイルをバックグラウンド実行
        se.execute_process_in_parallel(sub_setting.dir,sub_setting.exec_filename, sub_setting.config_filename)

        # 受信を待つ
        time.sleep(sub_setting.initial_wait_time_sec)

        is_even = True
        count = 0
        max_int = sys.maxsize
        while(True): 
            # while ループ 1回目は even
            # even(True):1 odd(False):0
            is_even = count % 2 == 0

            # 子プロセスの処理 : 心拍データを擬似心電図設定に反映 + 擬似心電図生成
            pid = os.fork()
            if pid == 0:

                cpr.child_process_routine(is_even, 
                                          pub_setting, gen_setting, sub_setting, dev_shm_setting, heart_json_setting)            
                os._exit(0)

            # 親プロセスの処理 : 擬似心電図 MQTT Publish
            else:
                # C++ 実行ファイルを実行
                se.execute_bash_script_or_shell_script(pub_setting.dir, pub_setting.exec_filename)

                # 子プロセスの終了を待つ
                os.waitpid(pid, 0)

            count = count % max_int
            count += 1            
    
    except KeyboardInterrupt:        
        print("\nKeyboardInterrupt caught. The program terminated.\n")

    
if __name__ == "__main__":
    main()


    