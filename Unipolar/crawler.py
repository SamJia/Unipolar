#!/bin/python
# -*- coding:utf8 -*-

import urllib2
import time
from threading import Thread
from Queue import Queue
from time import sleep


q = Queue()
NUM = 10

base_url = "http://www.littlegolem.net/servlet/sgf/%s/game%s.txt"
error = open("error.txt","w")

def donwload(url,id):
		content = urllib2.urlopen(url,timeout=5).read()
		if "SZ[13]" not in content:
			print "Passing", id
			return
		f = open(id+'.sgf',"w")
		f.write(content)
		f.close()
		print "Done",id

def working():
	while True:
		arguments = q.get()
		try:
			donwload(arguments[0], arguments[1])
		except:
			print "FATAL", arguments[1]
			error.write(arguments[1]+'\n')
		sleep(1)
		q.task_done()

    
if __name__ == '__main__':
	print "Loading..."
	for i in range(372808,1729607):
		id = str(i).zfill(7)
		url = base_url%(id,id)
		q.put([url,id])
	print "Begin to process..."
	for i in range(NUM):
		t = Thread(target=working)
		t.setDaemon(True)
		t.start()
	q.join()

	
