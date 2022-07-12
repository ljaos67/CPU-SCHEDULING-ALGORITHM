# CPU-SCHEDULING-ALGORITHM
a program in C++ or C which simulates CPU scheduling in an operating system
There is only one CPU. The scheduling algorithm you will implement is
FCFS. You can implement Round Robin for extra credits. You are suggested but not required to use
standard template library (STL) data structures such as vector and deque.
This program is lengthier than previous assignments. Please allocate sufficient time.

Assumptions:
(a) We will assume the processes engage in CPU bursts, Input bursts
and Output bursts, ignoring other interrupts.
(b) We will assume that all processes are doing Input through the
same device which can process one Input burst at a time.
(b) We will assume that all processes are doing Output through the
same device which can process one Output burst at a time.
(c) We will assume the system starts out with no processes active.
There may be processes ready to start at once.

Data structures:
You will need a struct or class to represent one process.
The program requires 4 queues: Entry, Ready, Input and Output. You can use
STL deque for the queues (unless you prefer to write your own implementation
of queue). The items stored on the queues are pointers to processes. You
can think of Entry queue as the queue where the processes reside (e.g. on
disk swap space) and they are loaded into memory at different times by OS
when capacity allows.

You can also have variables Active, IActive and OActive (pointers to
processes), which points to the active processes on the CPU, the input device
and the output device, respectively.

Constants:
You can declare the following constants:
MAX_TIME is an integer = length of the whole run. Use the value 500.
IN_USE is an integer = maximum number of processes that can be in play at
once (that is, Active/IOActive processes if any, plus those that are in
Ready/IO queues). Use the value 5.

2
HOW_OFTEN is an integer indicating how often to reprint the state of
the system. Use the value 25.
You may declare some other optional constants. They are not necessary if you choose to use STL:
QUEUE_SIZE is an integer guaranteed larger than the maximum number of
items any queue will ever hold. Use the value 20.
ARRAY_SIZE is an integer = size of the array to define in a process. It is
the maximum number of bursts for a process. Use the value 10.
Feel free to add more constants as you see fit.

The Process data structure:
A process needs to contain (at least) the following data:
• ProcessName is the name of the process, a string.
• ProcessID is an integer, the ID number for the process. This is assigned
by the system (i.e., your program). Use consecutive values such as 101,
102, 103, etc.

• History is an array or vector of pairs of the form (letter, value). They
are from the supplied input file, described below.
• Sub is a subscript into the array/vector History
• CPUTimer counts clock ticks for the process until it reaches
the end of the CPU burst for FCFS (or end of the quantum for RR).
• IOTimer counts clock ticks for the process until it reaches the end of the
I/O burst. You need two variables for I and O respectively.
• CPUTotal accumulates the number of clock ticks the process spends as
Active.
• IOTotal accumulates the number of clock ticks the process spends as
IOActive. You need two variables.
• CPUCount counts the number of completed CPU bursts for this process.
• IOCount counts the number of completed I/O bursts for this process. You
need two variables.

You may need more variables for the purpose of statistics summary. You can
change the names of variables.
Programming Requirement:
The events are governed by a timer which is simply counting clock
3
ticks. (For our purposes, a clock tick might be one millisecond).
The structure of the program is that a process moves from the Entry
queue to the Ready queue, eventually becomes Active, is moved to the
Input queue or the Output queue or back to the Ready queue as appropriate,
and eventually terminates. We will accumulate various statistics about each
process and some global statistics about the run as a whole.

1. Read the input file. Create processes and store them in
the Entry queue.
2. When processing starts, we have some initializations and then a large
loop.
3. We have a main loop. This loop represents the scheduler in the OS. This
contains most of the logic for this assignment.

It starts with Timer = 0. Timer is incremented at the bottom of the loop,
and the loop ends when Timer reaches MAX_TIME or there are no processes left
(all queues empty, Active, IActive and Oactive all NULL (0)).
At this point, you need to decide what to do in each case:
Compare the arrival time of the first few Processes in the Entry queue to
Timer. If Timer > arrival time and the total number of processes in play <
IN_USE, push the Process onto the Ready queue. You may initially push
several processes onto the queue, but later it will normally be one at a
time.

If we do not have an Active process, we need to look for one in the Ready
queue. If it is empty and the number of processes in the cycle is less than
IN_USE, obtain a process from the Entry queue (unless it is empty).
If we still do not have an Active process, there is little to
do on this iteration of the loop: idle time.
If we do have an Active process, what could happen to it?
--- It could come to the end of a CPU burst which is followed by an
Input or Output burst.
--- It could come to the end of a CPU burst which is followed by
termination of the process.
--- It could come to the end of a time slice but not yet the
end of a CPU burst (for Round Robin).
If we do not have an Iactive process, we need to look for one in the
Input queue. (It might be empty.)
If we have an Iactive process, it may come to the end of its
Input burst, in which case it is moved to the Ready queue.
If we do not have an Oactive process, we need to look for one in the
Output queue. (It might be empty.)
4

If we do have an Oactive process, it may come to the end of its Output burst,
in which case it is moved to the Ready queue.
4. After the main loop, print summary data about the whole run, such as
these (you may think of more):
--- What was the value of the Timer at the end?
--- How many processes terminated normally?
--- What was left in each queue at the end?
--- How much time was spent doing nothing (idle time)?
--- What was the average waiting time for all the terminated processes? For
this assignment, waiting time is defined as they are in the ready queue or
I/O queue (but not active).
This description leaves out a few details, such as initializing the
variables. You may in fact need a few more variables. You may need
a function (e.g. Terminate()) that summarizes the statistics and perhaps
more.
As the program runs, print output about major events such as when a
process enters the cycle from the Entry queue or when a process
terminates. When a process terminates, print what we know about it, such as:

--- how many CPU bursts occurred
--- how many Input bursts occurred
--- how many Output bursts occurred
--- how much time was spent in the CPU
--- how much time was spent in Input
--- how much time was spent in Output
--- how much time was spent in waiting (in ready queue or I/O queue)

Whenever the timer is a multiple of HOWOFTEN, print out the state of the
system:

--- the ID number of the Active process (if there is one)
--- the ID number of the Iactive process (if there is one)
--- the ID number of the Oactive process (if there is one)
--- the contents of the Entry, Ready, Input and Output queues

You will need to invent the report format for all this data. Put some effort
into it and make it easy to read.
Notice that there may be occasions when all processes are doing Input or
Output and we have idle time for the CPU.
Input File:
This file contains the processes: their names, arrival times and their CPU/IO bursts.
Each process is represented in the input file by two lines.
5

The first line contains:
--- a string = the name of this process
--- a nonnegative integer = the arrival time at which the request for
this process was submitted

The second line contains a sequence of pairs of the form:
letter, value
where letter = 'C' for a CPU burst, 'I' for an input burst, 'O' for an output
burst, or ‘N’ for the end of all bursts and value = the number of time units
for this burst.

CPU burst is the first burst for a process. There are not two consecutive
bursts of the same kind.

The last line has process name = "STOPHERE" and should not be
included. It simply serves as a delimiter.

Small example input files and expected output files are supplied with the assignment. In addition, another input file
“input.txt” is supplied, which you will bundle together with your code for the purpose of grading. TA may also use
other input files. Make your program taking in the input file name as the first command line argument.
