#! python2
#-*- coding=utf-8 -*-
import os, re

include_re = re.compile(r'\s*#include\s*"(.*)"\s*')
makefile = open('Makefile', 'w')
include_dict = {}

for root, dirs, filenames in os.walk("."):
	for filename in filenames:
		origin_name, suffix = os.path.splitext(filename)
		if suffix == ".cpp" or suffix == ".cc":
			include_dict[origin_name] = set()
			check_list = []
			check_list.append(filename)
			while check_list:
				fp = open(os.path.join(root, check_list[0]))
				for line in fp:
					include_match = include_re.match(line)
					if include_match:
						header_name = include_match.group(1)
						if include_match.group(1) not in include_dict[origin_name]:
							include_dict[origin_name].add(header_name)
							check_list.append(header_name)
				fp.close()
				check_list.remove(check_list[0])
value_dict = {}
value_dict['cc'] = 'g++'
value_dict['exe'] = 'main'
value_dict['std'] = 'c++11'
value_dict['objects'] = ' '.join(map(lambda x: x + '.o', include_dict.keys()))

for k, v in value_dict.items():
	makefile.write('%s = %s\n' % (k, v))
makefile.write('''
$(exe) : $(objects)
			$(cc) -o $(exe) $(objects)

''')

for k, v in include_dict.items():
	makefile.write('%s : %s\n' % (k + '.o', ' '.join(v)))
	makefile.write('\t$(cc) -std=$(std) -c %s.cpp\n' % k)
makefile.write('''
clean : 
		rm $(objects) $(exe)
''')