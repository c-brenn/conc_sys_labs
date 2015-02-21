from subprocess import Popen, PIPE, STDOUT
import sys

if len(sys.argv) < 2:
	print "Like this pls: python run_pc.py <number_of_consumer_threads>"
	exit(1)

num = int(sys.argv[1])

#compile source
cmd = "gcc -o pc prod_con.c -pthread"
p = Popen(cmd, shell=True, stdin=PIPE, stdout=PIPE, 
	    stderr=STDOUT, close_fds=True)
p.wait() #wait for compiling to complete

cmd = "./pc " + str(num)
p = Popen(cmd, shell=True, stdin=PIPE, stderr=STDOUT, close_fds=True)
p.wait() #wait for compiling to complete



