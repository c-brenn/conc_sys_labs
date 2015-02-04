import matplotlib.pyplot as pyplot
import json
import sys

#get the name of the file containing
#the results to be plotted
file_name = sys.argv[1]

#open the file and load the contents as JSON
data = open(file_name, 'r')
data = data.read()
results = json.loads(data)

#extract the x and y values to plot
x = results['x']
y = results['y']
#get the number of cores
cpu_count = results['cpu_count']

#plot the x values against the y values
pyplot.plot(x,y)
#label the axes
pyplot.ylabel('time (sec)')
pyplot.xlabel('threads used')
#set the scale on the axes
pyplot.axis([1, len(x), 0, max(y) + min(y)], 'o')
#add a vertical line at 'cpu_count'
pyplot.axvline(cpu_count, color='r')
#add a title and display the plot
pyplot.title('A plot of time vs threads used to calculate pi')
pyplot.show()