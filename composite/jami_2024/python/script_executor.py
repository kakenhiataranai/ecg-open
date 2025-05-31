
import os
import sys
import subprocess


# Bashスクリプトあるいはシェルスクリプトを実行する関数
def execute_bash_script_or_shell_script(dir, filename):
    try:
        file_path = os.path.join(dir, filename)
        subprocess.run(["bash", file_path], check=True, cwd=dir)

    except subprocess.CalledProcessError as e:
        print(f"Error executing the script: {e}")
        sys.exit(1)

# ファイルをバックグラウンドプロセスで実行する関数
# この関数で実行する C++ の実行ファイルはconfigファイル１つの想定
def execute_process_in_parallel(dir, exec_filename, config_filename):
    exec_path = os.path.join(dir, exec_filename)
    conf_path = os.path.join(dir, config_filename)
    
    try:
        if not os.path.exists(exec_path):
            raise FileNotFoundError(f"Exec file '{exec_path}' not found.")
        if not os.path.exists(conf_path):
            raise FileNotFoundError(f"Config file '{conf_path}' not found.")
        
        if not os.path.isdir(dir):
            raise NotADirectoryError(f"'{dir}' is not a directory.")
        
        if not os.access(exec_path, os.X_OK):
            raise PermissionError(f"No permission to execute '{exec_path}'.")
        
        subprocess.Popen([exec_path, conf_path])
    
    except (FileNotFoundError, NotADirectoryError, PermissionError) as e:
        print(f"Error: {e}")
        sys.exit(1)