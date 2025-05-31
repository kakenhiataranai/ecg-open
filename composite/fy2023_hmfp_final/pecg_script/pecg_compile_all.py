# coding: utf-8
import configparser
import os
import sys
import script_executor as se

# 設定ファイル名
CONFIG_FILENAME = 'pecg_config.ini'

def main():

    config_ini_path = CONFIG_FILENAME
    config_ini = configparser.ConfigParser()

    # 指定したiniファイルが存在しない場合 プログラムを終了
    if not os.path.exists(config_ini_path):
        print(f"Error: Configuration file '{config_ini_path}' does not exist")
        sys.exit(1)
    
    config_ini.read(config_ini_path, encoding='utf-8')


    # 擬似心電図生成C++プログラムをコンパイル
    gen_dir = config_ini['GENERATE']['dir']    
    gen_compile_filename = config_ini['GENERATE']['compile_filename'] 
    se.execute_bash_script_or_shell_script(gen_dir, gen_compile_filename)
    print(gen_dir + gen_compile_filename + '  executed.')

    # 擬似心電図データ送信C++プログラムをコンパイル
    pub_dir = config_ini['PUBLISH']['dir']    
    pub_compile_filename = config_ini['PUBLISH']['compile_filename'] 
    se.execute_bash_script_or_shell_script(pub_dir, pub_compile_filename)
    print(pub_dir + pub_compile_filename + '  executed.')

    # 心拍センサーデータ受信C++プログラムをコンパイル
    sub_dir = config_ini['SUBSCRIBE']['dir']    
    sub_compile_filename = config_ini['SUBSCRIBE']['compile_filename'] 
    se.execute_bash_script_or_shell_script(sub_dir, sub_compile_filename)
    print(sub_dir + sub_compile_filename + '  executed.')

    print()

    
if __name__ == "__main__":
    main()


    