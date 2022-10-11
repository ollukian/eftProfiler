import sys


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
            np_name = np_name_quoted[1:-1]
            print(f"np name: [{np_name}]")
            return np_name


def get_poi_name_from_list(lines : list):
    for this_line in lines:
        if "poi_name" in this_line:
            line_components = this_line.split()
            poi_name_quoted = line_components[1]
            poi_name = poi_name_quoted[1:-1]
            print(f"poi name: [{poi_name}]")
            return poi_name


def print_res_to_file(lines : list):
    print("to create file: {tmp} and to put inside:")
    for line in lines:
        print(line)
    print("end of the file")

    np_name = get_np_name_from_list(lines)
    poi_name = get_poi_name_from_list(lines)
    return


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



for filename in sys.argv[1:]:
    print(f"received: {filename} - check if it contains results...")
    if contains_result(filename):
        print(f"* {filename} contain results, extract if")
        data_one_file = get_result_one_file(filename)
        print_res_to_file(data_one_file)



