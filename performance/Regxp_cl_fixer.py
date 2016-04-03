# This program fix Regxp.cl according to this instruction:
# 
# 3. Open re.cpp and replace functions in Regxp.cl
#   a. replace content of classify_bytes_do_block (from do to while)
#   b. replace content of demo_do_block
#   c. replace lex structure

import sys

def extract_code_content(start_sign, stop_sign, source_file):	
	extract = []
	flag = False
	for line in open(source_file, 'r'):	
		if start_sign in line:
			flag = True

		if flag:			
			extract.append(line)

		if flag and stop_sign in line:
			flag = False

	return extract

if len(sys.argv) < 3:
	print "Usage: python <this_script> <path to re.cpp> <path to Regxp.cl.template>"
	exit()
else:
	source_file = sys.argv[1]
	template_file = sys.argv[2]
	print "Generating Regxp.cl from %s" % source_file

classify_bytes_do_block = extract_code_content("#define classify_bytes_do_block", "} while (0)", source_file)[2:-1]
demo_do_block = extract_code_content("#define demo_do_block", "} while (0)", source_file)[2:-1]
lex_definition = extract_code_content("struct Lex {", "};", source_file)[1:-1]

outf = open(template_file.replace('.template', ''), 'w')
for line in open(template_file):
	if "{{{@classify_bytes_do_block}}}" in line:
		outf.write(''.join(classify_bytes_do_block))
	elif "{{{@demo_do_block}}}" in line:
		outf.write(''.join(demo_do_block))
	elif "{{{@lex_definition}}}" in line:
		outf.write(''.join(lex_definition))
	else:
		outf.write(line)


