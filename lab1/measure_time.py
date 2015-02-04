import os
import re
import sys
from subprocess import Popen, PIPE, STDOUT
import multiprocessing as mp
import math
import json
import time as t

x = []
y = []

#compile subprocess code
cmd = "gcc -o leibniz leibniz.c -lm -pthread"
p = Popen(cmd, shell=True, stdin=PIPE, stdout=PIPE, 
	    stderr=STDOUT, close_fds=True)
p.wait() #wait for compiling to complete


# this loop runs 'leibniz.c' with n threads, where n iterates
# from 1 to max_threads. The runtime of each subprocess is measured
# to increase accuracy the subprocess is run 'samples' times for each
# n and the time taken is averaged

samples = 3
max_threads = int(1.5 * mp.cpu_count())
for i in range(1, max_threads):
	cmd = "./leibniz " + str(i)
	time = 0
	#sample the runtime of the subprocess with i threads
	for j in range(samples):
		start = t.time()
		p = Popen(cmd, shell=True, stdin=PIPE, stdout=PIPE, 
		    stderr=STDOUT, close_fds=True)
		p.wait()
		end = t.time()
		time = time + end - start
	#get the average time taken
	time = time / samples
	#store the time and the number of threads for graphing
	x.append(i)
	y.append(time)

#store the results and the number of cores as JSON
results = json.dumps({
	'x' : x,
	'y' : y,
	'cpu_count' : mp.cpu_count()
	});

#write the JSON rep of the results to a file
#the filename is passed as a command line arg
file_name = sys.argv[1]
output = open(file_name, 'w')
output.write(results)
output.flush()
output.close()
