import os
import sys
import json
import fcntl


def create_initial_json(dir, filename, key_0, value_0, key_1, value_1, key_2, value_2):

    data = {
        key_0: value_0,
        key_1: value_1,
        key_2: value_2
    }
    json_path = os.path.join(dir, filename)
    with open(json_path, "w") as json_file:
        json.dump(data, json_file)


def max_function_in_str(str_value, lower_bound):
    str_in_double = float(str_value)
    positive_str_in_double= max(str_in_double, lower_bound)
    return str(positive_str_in_double)


def read_heart_rate_json(dir, filename, heart_rate_key, normal_key, heart_rate_lower_bound):
    try:
        file_path = os.path.join(dir, filename)
        with open(file_path, "r") as file:

            # ファイルに共有の読み取りロックを取得
            fcntl.flock(file.fileno(), fcntl.LOCK_SH)
              
            # JSONのオブジェクト
            json_obj = json.load(file) 

            # ロックを解除            
            fcntl.flock(file.fileno(), fcntl.LOCK_UN)

            heart_rate = json_obj[heart_rate_key]
            normal = json_obj[normal_key]

        return max_function_in_str(heart_rate, heart_rate_lower_bound), normal

    except FileNotFoundError:
        print(f"{file_path}  was not found.")
        sys.exit(1)

    except Exception as e:
        print("Error:", e)
        sys.exit(1)
