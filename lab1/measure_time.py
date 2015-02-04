import os
import re
import sys
from subprocess import Popen, PIPE, STDOUT
import multiprocessing as mp
import math
import json

x = []
y = []

#compile subprocess code
cmd = "gcc -o leibniz leibniz.c -lm -pthread"
p = Popen(cmd, shell=True, stdin=PIPE, stdout=PIPE, 
	    stderr=STDOUT, close_fds=True)
p.wait() #wait for compiling to complete

samples = 4

max_threads = int(1.5 * mp.cpu_count())

for i in range(1, max_threads):
	cmd = "time -p ./leibniz " + str(i)
	time = 0
	for j in range(samples):
		p = Popen(cmd, shell=True, stdin=PIPE, stdout=PIPE, 
		    stderr=STDOUT, close_fds=True)
		time = time + float(re.findall("\d+.\d+", p.stdout.readline())[0])
	time = time / samples
	x.append(i)
	y.append(time)

results = json.dumps({
	'x' : x,
	'y' : y,
	'cpu_count' : mp.cpu_count()
	});

file_name = sys.argv[1]
output = open(file_name, 'w')
output.write(results)
output.flush()
output.close()
