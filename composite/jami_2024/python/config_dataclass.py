import os
import sys
import configparser
from dataclasses import dataclass
#import bool_string_converter as bsconv


# .iniファイル [AUTO_EXE] のデータクラス
@dataclass(frozen=True)
class AutoExeSetting:
    dir: str
    compile_filename: str
    exec_filename: str
    config_filename: str
    plot_original_filename: str
    plot_rename_filename_wo_extension: str
    plot_rename_filename_extention: str
    lead_key: str

def read_config_auto_exe(filename):
    config = configparser.ConfigParser()
    config.read(filename)
    return AutoExeSetting(
        dir = config['AUTO_EXE'].get('dir'),
        compile_filename = config['AUTO_EXE'].get('compile_filename'),
        exec_filename = config['AUTO_EXE'].get('exec_filename'),
        config_filename = config['AUTO_EXE'].get('config_filename'),
        plot_original_filename = config['AUTO_EXE'].get('plot_original_filename'),
        plot_rename_filename_wo_extension = config['AUTO_EXE'].get('plot_rename_filename_wo_extension'),
        plot_rename_filename_extention = config['AUTO_EXE'].get('plot_rename_filename_extention'),        
        lead_key = config['AUTO_EXE'].get('lead_key')
    )


def read_config(filename):
    if not os.path.exists(filename):
        print(f"Error: Configuration file '{filename}' does not exist")
        sys.exit(1)
    auto_exe_setting = read_config_auto_exe(filename)

    return auto_exe_setting