import os
import re
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

samples = 3

max_threads = mp.cpu_count() + 3

for i in range(1, max_threads):
	cmd = "time -p ./leibniz " + str(i)
	p = Popen(cmd, shell=True, stdin=PIPE, stdout=PIPE, 
	    stderr=STDOUT, close_fds=True)
	time = float(re.findall("\d+.\d+", p.stdout.readline())[0])
	x.append(i)
	y.append(time)

results = json.dumps({
	'x' : x,
	'y' : y,
	'cpu_count' : mp.cpu_count()
	});
output = open('results.json', 'w')
output.write(results)
output.flush()
output.close()
