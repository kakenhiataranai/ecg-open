
import os
import sys
import shutil


def exists_file_in_dir(dir, filename):
    file_path = os.path.join(dir, filename)
    if os.path.exists(file_path):
        return True
    else:
        return False


def create_file_in_dir(dir, filename):
    try:
        file_path = os.path.join(dir, filename)
        with open(file_path, 'w') as file:
            pass
        print(f"{filename} created in {dir}")
    except Exception as e:
        print(f"Error creating {filename} in {dir}: {e}")
        sys.exit(1)


def delete_file_in_dir(dir, filename):
    try:
        file_path = os.path.join(dir, filename)
        os.remove(file_path)
        print(f"{file_path}  deleted successfully.")
    except FileNotFoundError:
        print(f"{file_path}  not found.")
        #sys.exit(1)
    except Exception as e:
        print(f"Error deleting {filename} in {dir}: {e}")
        sys.exit(1)


def copy_file_in_dir(dir, filename, add):
    try:        
        name, extension = os.path.splitext(os.path.basename(filename))
        file_path = os.path.join(dir, filename)        
        filename_copy = name + add + extension 
        file_path_copy = os.path.join(dir, filename_copy)
        shutil.copy(file_path, file_path_copy)

    except FileNotFoundError:
        print(f"Error: {filename} in {dir} not found.")
        sys.exit(1)

    except Exception as e:
        print(f"Error copying file: {e}")


def replace_string_value_in_config(dir, config_filename, key, new_value):
    try:
        file_path = os.path.join(dir, config_filename)

        with open(file_path, 'r') as file:
            lines = file.readlines()

        search_string= f'{key}='
        new_string= f'{key}="{new_value}";\n'
        modified_lines = [new_string if search_string in line else line for line in lines]
        
        with open(file_path, 'w') as file:
            file.writelines(modified_lines)

    except Exception as e:
        print(f"Error modifying file: {e}")
        sys.exit(1)


def replace_value_in_config(dir, config_filename, key, new_value):
    try:
        file_path = os.path.join(dir, config_filename)

        with open(file_path, 'r') as file:
            lines = file.readlines()

        search_string= f'{key}='
        new_string= f'{key}={new_value};\n'
        modified_lines = [new_string if search_string in line else line for line in lines]
        
        with open(file_path, 'w') as file:
            file.writelines(modified_lines)

    except Exception as e:
        print(f"Error modifying file: {e}")
        sys.exit(1)


def rename_file(dir, filename, new_filename):
    try:    
        old_file_path = os.path.join(dir, filename)       
        new_file_path = os.path.join(dir, new_filename)
        os.rename(old_file_path, new_file_path)

    except FileNotFoundError:
        print(f"Error: '{filename}' not found in directory '{dir}'.")
        sys.exit(1)
    except Exception as e:
        print(f"Error renaming file: {e}")
        sys.exit(1)  