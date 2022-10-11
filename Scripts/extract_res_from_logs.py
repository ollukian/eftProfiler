import sys


def contains_result(name : str):
    with open(name) as file_to_check:
        for this_line in file_to_check:
            if "duplicate to the console:":
                return True
    return False


def print_res_to_file(lines : list):
    print("to create file: {tmp} and to put inside:")
    for line in lines:
        print(line)
    print("end of the file")
    return


def get_result_one_file(name : str):
    is_inside_res = False
    res = list()
    with open(name) as file_to_check:
        for line in file_to_check:
            if '{' in line:
                is_inside_res = True
            elif '}' in line:
                is_inside_res = False
                # print_res_to_file(res)
                return res

            if is_inside_res:
                res.append(line)


for filename in sys.argv:
    print(f"received: {filename} - check if it contains results...")
    if contains_result(filename):
        print(f"* {filename} contain results, extract if")
        data_one_file = get_result_one_file(filename)
        print_res_to_file(data_one_file)



