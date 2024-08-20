import os
import json

def dir_check(dir_name):
    dir_name = [dir_name] if not type(dir_name) == list else dir_name
    dir_path = os.path.join(*dir_name)
    if os.path.isdir(dir_path):
        pass
    else:
        os.mkdir(dir_path)

def file_check(file_name, dir_name=''):
    assert type(file_name) == str
    dir_name = [dir_name] if ((type(dir_name) != list) and (dir_name)) else dir_name
    full_path = os.path.join(*dir_name, file_name)
    return os.path.isfile(full_path)

def json_save(data, file_name, dir_name=''):
    dir_name = [dir_name] if ((type(dir_name) != list) and (dir_name)) else dir_name
    assert type(file_name) == str
    file_name = file_name + '.json' if not file_name.endswith('.json') else file_name
    full_path = os.path.join(*dir_name, file_name)
    with open(full_path, 'w') as fp:
        json.dump(data, fp)

def json_load(file_name, dir_name=''):
    dir_name = [dir_name] if ((type(dir_name) != list) and (dir_name)) else dir_name
    file_name = file_name + '.json' if not file_name.endswith('.json') else file_name
    full_path = os.path.join(*dir_name, file_name)
    with open(full_path) as fp:
        return json.load(fp)

def load_config(args):
    configs = json_load(args.load_config, args.config_location)
    for parameters in configs:
        args.__setattr__(parameters, configs[parameters])
    return args