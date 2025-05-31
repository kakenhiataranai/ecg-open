import os
import sys
import configparser
from dataclasses import dataclass
import bool_string_converter as bsconv


# .iniファイル [PUBLISH] のデータクラス
@dataclass(frozen=True)
class PublishSetting:
    dir: str
    compile_filename: str
    exec_filename: str
    config_filename: str
    rename_target_key: str
    rename_value_even: str
    rename_value_odd: str

def read_config_publish(filename):
    config = configparser.ConfigParser()
    config.read(filename)
    return PublishSetting(
        dir = config['PUBLISH'].get('dir'),
        compile_filename = config['PUBLISH'].get('compile_filename'),
        exec_filename = config['PUBLISH'].get('exec_filename'),
        config_filename = config['PUBLISH'].get('config_filename'),
        rename_target_key = config['PUBLISH'].get('rename_target_key'),
        rename_value_even = config['PUBLISH'].get('rename_value_even'),
        rename_value_odd = config['PUBLISH'].get('rename_value_odd')
    )


# .iniファイル [GENERATE] のデータクラス
@dataclass(frozen=True)
class GenerateEcgSetting:
    dir: str
    compile_filename: str
    exec_filename: str
    waveform_config_filename : str
    json_filename: str
    heart_rate_bpm_key: str
    heart_rate_normal_key: str

def read_config_generate_ecg(filename):
    config = configparser.ConfigParser()
    config.read(filename)
    return GenerateEcgSetting(
        dir = config['GENERATE'].get('dir'),
        compile_filename = config['GENERATE'].get('compile_filename'),
        exec_filename = config['GENERATE'].get('exec_filename'),
        waveform_config_filename = config['GENERATE'].get('waveform_config_filename'),
        json_filename = config['GENERATE'].get('json_filename'),
        heart_rate_bpm_key = config['GENERATE'].get('heart_rate_bpm_key'),
        heart_rate_normal_key = config['GENERATE'].get('heart_rate_normal_key')
    )


# .iniファイル [SUBSCRIBE] のデータクラス
@dataclass(frozen=True)
class SubscribeSetting:
    dir: str
    compile_filename: str
    exec_filename: str 
    config_filename: str
    initial_wait_time_sec: float
    heart_json_filename: str

def read_config_subscribe(filename):
    config = configparser.ConfigParser()
    config.read(filename)
    return SubscribeSetting(
        dir = config['SUBSCRIBE'].get('dir'),
        compile_filename = config['SUBSCRIBE'].get('compile_filename'),
        exec_filename = config['SUBSCRIBE'].get('exec_filename'),
        config_filename = config['SUBSCRIBE'].get('config_filename'),
        initial_wait_time_sec = float(config['SUBSCRIBE'].get('initial_wait_time_sec')),
        heart_json_filename = config['SUBSCRIBE'].get('heart_json_filename')
    )


# .iniファイル [DEV_SHM] のデータクラス
@dataclass(frozen=True)
class DevShmSetting:
    dir: str

def read_config_dev_shm(filename):
    config = configparser.ConfigParser()
    config.read(filename)
    return DevShmSetting(
        dir = config['DEV_SHM'].get('dir')
    )


# .iniファイル [HEART_RATE_JSON] のデータクラス
@dataclass(frozen=True)
class HeartRateJsonSetting:
    initial_bpm: str
    initial_msec_since_epoch: str
    initial_normal: bool
    bpm_key: str
    msec_since_epoch_key: str
    normal_key: str
    bpm_lower_bound: float

def read_config_heart_rate_json(filename):
    config = configparser.ConfigParser()
    config.read(filename)
    return HeartRateJsonSetting(
        initial_bpm = config['HEART_RATE_JSON'].get('initial_bpm'),
        initial_msec_since_epoch = config['HEART_RATE_JSON'].get('initial_msec_since_epoch'),
        initial_normal = bsconv.string_to_bool(config['HEART_RATE_JSON'].get('initial_normal')),
        bpm_key = config['HEART_RATE_JSON'].get('bpm_key'),
        msec_since_epoch_key = config['HEART_RATE_JSON'].get('msec_since_epoch_key'),
        normal_key = config['HEART_RATE_JSON'].get('normal_key'),
        bpm_lower_bound = float(config['HEART_RATE_JSON'].get('bpm_lower_bound'))
    )


def read_config(filename):
    if not os.path.exists(filename):
        print(f"Error: Configuration file '{filename}' does not exist")
        sys.exit(1)
    pub_setting = read_config_publish(filename)
    gen_setting = read_config_generate_ecg(filename)
    sub_setting = read_config_subscribe(filename)
    dev_setting = read_config_dev_shm(filename)
    heart_setting = read_config_heart_rate_json(filename)

    return pub_setting, gen_setting, sub_setting, dev_setting, heart_setting