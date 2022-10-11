import sys
import argparse
import shutil
from os import listdir
from os.path import isfile, join


def contains_result(name : str):
    with open(name) as file_to_check:
        for this_line in file_to_check:
            if "print to console:":
                return True
    return False


def get_np_name_from_list(lines : list):
    for this_line in lines:
        if "np_name" in this_line:
            line_components = this_line.split()
            np_name_quoted = line_components[1]
            np_name = np_name_quoted[1:-2]
            print(f"np name: [{np_name}]")
            return np_name


def form_filename(np : str, poi : str, worker_id : int):
    return "res__{}__worker_{}__{}.json".format(poi, worker_id, np)


def get_poi_name_from_list(lines : list):
    for this_line in lines:
        if "poi_name" in this_line:
            line_components = this_line.split()
            poi_name_quoted = line_components[1]
            poi_name = poi_name_quoted[1:-2]
            print(f"poi name: [{poi_name}]")
            return poi_name


def get_worker_id_from_filename(name : str):
    name_no_prefix = name.split('/')[-1]
    print(f"name: [{name}]")
    print(f"name no prefix: [{name_no_prefix}]")
    number_and_filetype = name_no_prefix.split('_')[-1]
    print(f"name with filetype: [{number_and_filetype}]")
    number = number_and_filetype.split('.')[0]
    print(f"number: [{number}]")
    return int(number)


def print_res_to_file(lines : list, worker_id : int):
    np_name = get_np_name_from_list(lines)
    poi_name = get_poi_name_from_list(lines)
    res_filename = form_filename(np_name, poi_name, worker_id)
    print(f"to create file: [{res_filename}] and to put inside:")

    is_json_started = False

    with open(res_filename, 'w') as fout:
        for line in lines:
            if '{' in line:
                is_json_started = True

            if is_json_started:
                fout.write(line)

    # for line in lines:
    #     if '{' in line:
    #         is_json_started = True
    #
    #     if is_json_started:
    #         print(line)
    return res_filename


def get_result_one_file(name : str):
    is_inside_res = False
    is_after_fixed_np_fit = False
    res = list()
    with open(name) as file_to_check:
        for line in file_to_check:
            if "after fixed np fit" in line:
                is_after_fixed_np_fit = True

            if 'print to console:' in line:
                is_inside_res = True
            elif "[Application]" in line:
                is_inside_res = False
            # elif '}' in line:
            #    is_inside_res = False
            #    # print_res_to_file(res)
            #    return res

            if is_inside_res and is_after_fixed_np_fit:
                res.append(line)
        return res


def process_one_file(filename : str, res_path : str):
    print(f"received: {filename} - check if it contains results...")
    if contains_result(filename):
        print(f"* {filename} contain results, extract if")
        data_one_file = get_result_one_file(filename)
        worker_id = get_worker_id_from_filename(filename)
        res_filename = print_res_to_file(data_one_file, worker_id)

        if res_path != '.':
            print(f"[INFO] copy file to {res_path}")
        shutil.copy2(res_filename, res_path + res_filename)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Extracts json logs from the log files, if the json results were not '
                                                 'printed')
    parser.add_argument('-f', '--file', type=str, help='name of the log file to take info from ', required=True)
    parser.add_argument('-d', '--dir', type=str, help='name of the folder to write info to')
    parser.add_argument('-m', '--multiple', help='To process all files from the folder')

    args = parser.parse_args()
    res_path = '.'

    if args.dir:
        res_path = args.dir

    if args.multiple:
        path = args.file
        print(f"[INFO] multiple files mode, process all files in the {path} folder")
        onlyfiles = [f for f in listdir(path) if isfile(join(path, f))]
        for file in onlyfiles:
            process_one_file(path + file, res_path)
    else:
        print(f"[INFO] onr file mode, process file: {args.file}")
        process_one_file(args.file, res_path)







