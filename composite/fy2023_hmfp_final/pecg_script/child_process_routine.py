import file_manipulation_lib as fmlib
import json_manipulation_lib as jmlib
import bool_string_converter as bsconv
import script_executor as se


# 子プロセスの処理をまとめたルーチン
def child_process_routine(is_even, pub_setting, gen_setting, sub_setting, dev_shm_setting, heart_json_setting):

    # 心拍の.json から心拍数[bpm]と正常/異常のbool値を取得 : 正常=True
    heart_rate_bpm, normal = jmlib.read_heart_rate_json(dev_shm_setting.dir, 
                                                        sub_setting.heart_json_filename, 
                                                        heart_json_setting.bpm_key, 
                                                        heart_json_setting.normal_key, 
                                                        heart_json_setting.bpm_lower_bound)

    # 擬似データ生成の config_gaussian_pecg.conf の 心拍数[bpm] を変更
    fmlib.replace_value_in_config(gen_setting.dir, 
                                  gen_setting.waveform_config_filename, 
                                  gen_setting.heart_rate_bpm_key, 
                                  heart_rate_bpm)

    # 擬似データ生成の config_gaussian_pecg.conf の 心拍数の正常/異常 を変更
    fmlib.replace_value_in_config(gen_setting.dir, 
                                  gen_setting.waveform_config_filename, 
                                  gen_setting.heart_rate_normal_key, 
                                  bsconv.bool_to_string(normal))

    # 擬似データ生成 C++ 実行ファイルを実行
    # pecg_gen.json を作成
    se.execute_bash_script_or_shell_script(gen_setting.dir, gen_setting.exec_filename) 
        

    # 擬似心電図データの .json が１つだと
    # 擬似データ生成処理時とpublish処理時でファイルアクセスが競合するため
    # publishする擬似心電図データの .json をカウンタの偶奇で切り替える
    if(is_even):
        # pecg_gen.json を odd_gen.json にリネーム
        fmlib.rename_file(dev_shm_setting.dir, gen_setting.json_filename, pub_setting.rename_value_odd)

        # C++ の publish の config の読み込み.jsonを odd_gen.json にリネーム
        fmlib.replace_string_value_in_config(pub_setting.dir, 
                                             pub_setting.config_filename,
                                             pub_setting.rename_target_key,
                                             pub_setting.rename_value_odd)

    else:
        # pecg_gen.json を even_gen.json にリネーム
        fmlib.rename_file(dev_shm_setting.dir, gen_setting.json_filename, pub_setting.rename_value_even)
                
        # C++ の publish の config の読み込み.jsonを even_gen.json にリネーム
        fmlib.replace_string_value_in_config(pub_setting.dir, 
                                             pub_setting.config_filename, 
                                             pub_setting.rename_target_key, 
                                             pub_setting.rename_value_even)
        