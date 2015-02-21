#Lab 2 : Poducers and Consumers

###What is this?

 The objective of this lab was to simulate transactions between a producer thread and multiple consumer threads using condition variables so as to avoid constant polling. The producer creates items and places them in a buffer for the consumers to use. As this behaviour is only being simulated the items created are just strings. The production/consumption of an item should take some time so the threads wait a pseudorandom amount of time when producing/consuming.
 

###Run

The main program is written in c. To compile and run it use the python program:

```
python run_pc.py <number of consumers>
```