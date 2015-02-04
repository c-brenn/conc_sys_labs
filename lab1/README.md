#Lab 1 : Concurrent Pi

###What is this?

 The objective of this lab was to write a program to calculate pi concurrently and to measure how the runtime changes as more/fewer threads are used.
 
###Dependencies

[MatPlotLib](http://matplotlib.org/) : used to plot the results

Install within a virtualenv

```
pip install python-matplotlib

```

###Run

Run measure_time.py to write the results to a file

```
python measure_time.py <file_name.json>
```
Run graph_results.py

```
python graph_results.py <file_name.json>
```