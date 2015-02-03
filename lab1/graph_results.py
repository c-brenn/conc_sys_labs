import matplotlib.pyplot as pyplot
import json

data = open('results.json', 'r')
data = data.read()
results = json.loads(data)

x = results['x']
y = results['y']
cpu_count = results['cpu_count']

pyplot.plot(x,y)
pyplot.ylabel('time (sec)')
pyplot.xlabel('threads used')
pyplot.axis([1, len(x) - 1, 0, max(y) + min(y)], 'o')
pyplot.axvline(cpu_count, color='r')
pyplot.title('A plot of time vs threads used to calculate pi')
pyplot.show()