#!/usr/bin/python
# -*- coding:utf8 -*-

import os

path = os.getcwd()
files = os.listdir(path)
output_80 = open("data/moves_80.dat","w")
output_40 = open("data/moves_40.dat","w")
output_20 = open("data/moves_20.dat","w")
output_10 = open("data/moves_10.dat","w")
offset = len("SO[http://www.littlegolem.com];")
cnt_80 = 0
cnt_40 = 0
cnt_20 = 0
cnt_10 = 0
def process(fname):
	f = open(fname,"r")
	content = f.readline()
	content = content[content.find("SO[http://www.littlegolem.com];")+offset:-1].split(";")
	print "Done", fname
	return content

for fname in files:
	if "sgf" in fname:
		res = process(fname)
		if len(res)>=80:
			output_80.write("#"+fname+"\n"+str(res)+'\n')
			cnt_80 += 1
		if len(res)>=40:
			output_40.write("#"+fname+"\n"+str(res)+'\n')
			cnt_40 += 1
		if len(res)>=20:
			output_20.write("#"+fname+"\n"+str(res)+'\n')
			cnt_20 += 1
		if len(res)>=10:
			output_10.write("#"+fname+"\n"+str(res)+'\n')
			cnt_10 += 1
	
print "Pre-processing completed!"
print cnt_10,cnt_20,cnt_40,cnt_80

