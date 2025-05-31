
def bool_to_string(value):
    if value is True:
        return 'true'
    elif value is False:
        return 'false'
    
def string_to_bool(value):
    if value == 'True':
        return True
    elif value == 'False':
        return False