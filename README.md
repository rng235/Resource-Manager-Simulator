# README - 11/19/2015

Resource Managers:
    FIFO (Optimistic) and Banker's
++++++++++++++++++++++++++++

Thank you for downloading this resource manager simulator.

Please see the FAQ below:

FAQ:
——————————————————————————————————————

1. How to run the program?
	a.	Before compiling and running please have the scheduler exe in the same directory as any input file that will be utilized.
	b.  Compiling should look something like g++ main.cpp ResourceManager.cpp Verbose.cpp Task.cpp
	c.	There are TWO possible arguments to pass into the program when running:
	    1. <input-filename>
	    2. --verbose <input-filename>
	d.	The program should run successfully and display the output on the terminal (standard output).
——————————————————————————————————————

2. What is verbose mode?
    - When verbose mode is activated, it will display all of the cycles involved
    in a particular algorithm. This is especially useful when debugging the program.
    It indicates the Timer, Task being processed, Activity of task, when a request can be granted
    and when a task is being terminated.

——————————————————————————————————————
Thank you for using this Resource Manager Simulator.

    Author: Ricardo Guntur (ricardo.guntur@nyu.edu)
    Course: Operating Systems -- Professor Allan Gottlieb.
    Semester: Fall 2015


