
with open("targets", "r") as f:
     lines = f.readlines()

lines_dictionary={}

for line in lines:
    one_line = line.split()
    line_key = one_line[0]
    line_value = one_line[1]
    lines_dictionary[line_key] = line_value

for key in lines_dictionary.keys():
    target_string = lines_dictionary[key]
    target_list = target_string.split(',')
    target_number = len(target_list)
    filename = key + '_' + str(target_number) + '_targets'
    vuln_file = open(filename, 'w')
    for vuln_target in target_list:
            vuln_file.write(vuln_target + '\n')
    vuln_file.close()
