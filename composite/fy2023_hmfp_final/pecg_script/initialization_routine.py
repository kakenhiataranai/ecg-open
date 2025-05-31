import os
import file_manipulation_lib as fmlib
import json_manipulation_lib as jmlib
import script_executor as se


# MQTT起動前のシステム初期設定の処理をまとめたルーチン
def initialization_routine(pub_setting, gen_setting, sub_setting, dev_shm_setting, heart_json_setting):

    # /dev/shm/files が存在しなければ作成
    if not os.path.exists(dev_shm_setting.dir):
        os.makedirs(dev_shm_setting.dir)
        print(dev_shm_setting.dir + ' was created.')

    # /dev/shm/files に pecg_gen_json, even_gen.json, odd_gen.json が存在したら削除しておく
    fmlib.delete_file_in_dir(dev_shm_setting.dir, gen_setting.json_filename)
    fmlib.delete_file_in_dir(dev_shm_setting.dir, pub_setting.rename_value_even)
    fmlib.delete_file_in_dir(dev_shm_setting.dir, pub_setting.rename_value_odd)

    # 擬似データ生成 実行ファイル読み込み 初回の .json を作成
    se.execute_bash_script_or_shell_script(gen_setting.dir, gen_setting.exec_filename)

    # pecg_gen.json を even_gen.json にリネーム
    fmlib.rename_file(dev_shm_setting.dir, gen_setting.json_filename, pub_setting.rename_value_even)

    # C++ の publish / config_pecg_pub.txt / JSON_FILE_NAME を even_gen.json にリネーム
    fmlib.replace_string_value_in_config(pub_setting.dir, 
                                         pub_setting.config_filename,
                                         pub_setting.rename_target_key, 
                                         pub_setting.rename_value_even)
    
    # Subscribe できないときの保険として心拍の.json を作成
    jmlib.create_initial_json(
        dev_shm_setting.dir, 
        sub_setting.heart_json_filename, 
        heart_json_setting.bpm_key, 
        heart_json_setting.initial_bpm,
        heart_json_setting.msec_since_epoch_key, 
        heart_json_setting.initial_msec_since_epoch, 
        heart_json_setting.normal_key,             
        heart_json_setting.initial_normal)