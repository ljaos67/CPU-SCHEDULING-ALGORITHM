/****************************************************************
   PROGRAM:   Assignment 4
   AUTHOR:    Leonart Jaos
   LOGON ID:  Z1911688
   DUE DATE:  03/30/2022

   FUNCTION:  Linux program in C++ which uses fork system call to 
			  start child processes.

   INPUT:     Input File: This file contains the processes: their names, arrival times and their CPU/IO bursts.

   OUTPUT:  major events such as when a process enters the cycle from the Entry queue or when a process
			terminates. When a process terminates, print what we know about it. Whenever the timer is a multiple of HOWOFTEN, print out the state of the
			system: 

   NOTES:     
****************************************************************/
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <deque>
#include <iterator>
#include <algorithm>

using namespace std;

int MAX_TIME = 500; // length of the whole run
int IN_USE = 5; // maximum number of processes that can be in play at once
int  HOW_OFTEN = 25; // is an integer indicating how often to reprint the state of the system
int QUEUE_SIZE = 20;

struct Pair
{
	char letter;
	int value;
};

class process
{
	public:
	string processName; //name of the process, a string.
	int arrivalTime = 0;
	int processID = 0; // the ID number for the process
	vector<Pair> History; // vector of pairs of the form (letter, value).
	int sub = 0; // subscript into the array/vector History	
	int sTime = 0;
	double CPUTimer = 0; //counts clock ticks for the process until it reaches the end of the CPU burst for FCFS
	double iTimer = 0; // counts clock ticks for the process until it reaches the end of the I burst.
	double oTimer = 0; // counts clock ticks for the process until it reaches the end of the O burst.
	double CPUTotal = 0; // accumulates the number of clock ticks the process spends as Active.
	double iActive = 0; // accumulates the number of clock ticks the process spends as IActive
	double oActive = 0; // accumulates the number of clock ticks the process spends as OActive
	double CPUCount = 0; // counts the number of completed CPU bursts for this process
	double iCount = 0; // counts the number of completed I bursts for this process.
	double oCount = 0; // counts the number of completed O bursts for this process.
	void printHistory()
	{
		for(unsigned long int i = 0; i < History.size(); i++)
		{
			cout << History[i].letter << "  " << History[i].value << "  ";
		}
		cout << endl;
	};
	
};

int main()
{
	// for references to objects
	process *container[QUEUE_SIZE];
	for(int i = 0; i < QUEUE_SIZE; ++i)
	{
		container[i] = new process();
	}
	process* cpu_active = NULL;
	process* out_active = NULL;
	process* in_active = NULL;
	deque<process*> Entry;
	deque<process*> Ready;
	deque<process*> Input;
	deque<process*> Output;
	deque<process*> Terminated;
	int Timer = 0;
	//bool firstCheck = false;
	int waitTime = 0;
	int pID = 0;
	int popCount = 0;
	int inPlay = 0;
	int keepCount = 0;
	string line;
	bool pLine = true;
	bool CPUwork = false;
	//bool idle = false; // When there is no process in CPU burst
	int idleCnt = 0;
	Pair exp;
	process tmp; 
	string str;
	// Read input
	cout << "Simulation of CPU Scheduling " << "\n\n";
	while(cin)
	{
		getline(cin, line);
		//cin.ignore();
		if(pLine)
		{
			
			tmp.processName = line.substr(0, line.find(' '));
			if(tmp.processName == "STOPHERE")
			{
				break;
			}
			else
			{
				tmp.arrivalTime = stoi(line.substr(line.find(' ') + 2, line.size()));
				pLine = false;
				//cout << tmp.processName << " " << tmp.arrivalTime << endl;
			}
		}
		else
		{
			// next line
			unsigned long int i = 0;
			while(i < line.size())
			{
				if(keepCount == 0 && line[i] != ' ')
				{
					// C, I, O, or N
					exp.letter = line[i];
					keepCount = 1;
				}
				else if(line[i] == ' ')
				{
					if(keepCount == 2)
					{
						exp.value = stoi(str);
						str = "";
						tmp.History.push_back(exp);
						keepCount = 0;
					}
				}
				else
				{
					// bulding burst number for C, I, O, or N
					str += line[i];
					keepCount = 2;
				}
				i++;
			}
			// initializations from input
			tmp.processID = pID;
			container[pID]-> processName = tmp.processName;
			container[pID]-> arrivalTime = tmp.arrivalTime;
			container[pID]-> processID = tmp.processID;
			container[pID]-> History.resize(tmp.History.size());
			container[pID]-> History = tmp.History;
			Entry.push_back(container[pID]);
			pID++;
			tmp.History.clear();
			pLine = true;
		}
	}
	// Main loop
	do 
	{

		// if ready empty
		if(cpu_active == NULL)
		{
			// search Ready queue 
			if(Ready.empty())
			{

				if(Entry.empty())
				{
					//idle = true;
					//idleCnt++;
				}
				else
				{
					for(unsigned long int i = 0; i < Entry.size(); i++)
					{
						if((Timer >= Entry[(int)i]->arrivalTime) &&(inPlay < IN_USE))
						{
							Ready.push_back(Entry[(int)i]);
							cout << "Process " << Entry[(int)i]->processID + 101
							<< " moved from the Entry Queue into the Ready Queue at time" << Timer
							<< "\n\n";
							
							popCount++;
							inPlay++;
							//found = true;
							//idle = false;
						}
					}
					// clear consumed processes from entry
					for(int i = 0; i < popCount; i++)
					{
						Entry.pop_front();
					}
					popCount = 0;
					// print state
					if(Timer % HOW_OFTEN == 0)
					{
						cout << "Status at time " << Timer << endl;
						cout << "Active is " << (cpu_active == NULL ? 0 : cpu_active->processID + 101) << endl;
						cout << "IActive is " << (in_active == NULL ? 0 : in_active->processID + 101) << endl;
						cout << "OActive is " << (out_active == NULL ? 0 : out_active->processID + 101) << endl;
						cout << "Contents of the Entry Queue:" << endl;
						for(unsigned long int i = 0; i < Entry.size(); i++)
						{
							cout << Entry[i]->processID + 101 << " ";
						}
						if(Entry.empty())
						{
							cout << "(empty)";
						}
						cout << endl;			
						cout << "Contents of the Ready Queue:" << endl;
						for(unsigned long int i = 0; i < Ready.size(); i++)
						{
							cout << Ready[i]->processID + 101 << " ";
						}
						if(Ready.empty())
						{
							cout << "(empty)";
						}
						cout << endl;
						cout << "Contents of the Input Queue:" << endl;
						for(unsigned long int i = 0; i < Input.size(); i++)
						{
							cout << Input[i]->processID + 101 << " ";
						}
						if(Input.empty())
						{
							cout << "(empty)";
						}
						cout << endl;
						cout << "Contents of the Output Queue:" << endl;
						for(unsigned long int i = 0; i < Output.size(); i++)
						{
							cout << Output[i]->processID + 101 << " ";
						}
						if(Output.empty())
						{
							cout << "(empty)";
						}
						cout << "\n\n";
					}

					cpu_active = Ready.front();
					Ready.pop_front();
					
					if(cpu_active->sub == 0)
					{
						cpu_active->sTime = Timer;
					}
				}
			}
			else // ready not empty
			{

				cpu_active = Ready.front();
				Ready.pop_front();
				if(cpu_active->sub == 0)
				{
					cpu_active->sTime = Timer;
				}
			}
		}
		
		if(cpu_active != NULL)// cpu not null
		{
			if((cpu_active->History[cpu_active->sub].value == 0))
			{
				cpu_active->CPUCount++;
				//switch to next
				cpu_active->sub++;
				if(cpu_active->History[cpu_active->sub].letter == 'I')
				{
					// letter = I
						for(int i = 0; i < QUEUE_SIZE; i++)
						{
							if(container[i] == cpu_active)
								Input.push_back(container[i]);
						}
						cpu_active = NULL;
				}
			
				else if(cpu_active->History[cpu_active->sub].letter == 'O')
				{
					// letter = O
						for(int i = 0; i < QUEUE_SIZE; i++)
						{
							if(container[i] == cpu_active)
								Output.push_back(container[i]);
						}
						cpu_active = NULL;				
				}
				else
				{
					// letter = N
					cout <<" Process " << cpu_active->processID << " has ended." << endl;
					cout << "Name:  " << cpu_active->processName << endl;
					cout << "Started at time " << cpu_active->sTime << " and ended at time " << Timer << endl; 
					cout << "Total CPU time = " << cpu_active->CPUTotal << " in " << cpu_active-> CPUCount<< 
					" CPU bursts" << endl;
					cout << "Total Input time = " << cpu_active->iActive << " in " << cpu_active-> iCount <<
					" Input bursts" << endl;
					cout << "Total Output time = " << cpu_active->oActive << " in " << cpu_active-> oCount <<
					" Output bursts" << endl;
					cout << "Time spent in waiting: " << cpu_active->CPUTimer + cpu_active->iTimer + cpu_active->oTimer << "\n\n"; 
					inPlay--;
					for(int i = 0; i < QUEUE_SIZE; i++)
					{
						if(container[i] == cpu_active)
							Terminated.push_back(container[i]);
					}
					cpu_active = NULL;
				}
			}
			else
			{
				//cpu_active->CPUCount++;
				CPUwork = true;
				cpu_active->CPUTotal++;
				cpu_active->History[cpu_active->sub].value--;
			}

		}
		for(unsigned long int j = 0; j < Ready.size(); j++)
		{
			Ready[(int)j]->CPUTimer+=1;
		}
		// set pointer to value
		if(in_active == NULL)
		{
				// search Ready queue 
			if(Input.empty())
			{
				// perhaps statistics?
			}
			else
			{
				in_active = Input.front();
				Input.pop_front();
			}
	
		}
		if(in_active != NULL) // active contains 
		{
			in_active->iActive++;
			if(((in_active->History[in_active->sub].value - 1) == 0))
			{
				in_active->iCount++;
				in_active->History[in_active->sub].value--;
				//switch to next
				in_active->sub++;
				if(in_active->History[in_active->sub].letter == 'C')
				{
					// letter = C
					Ready.push_back(in_active);
					in_active = NULL;
				}
				else
				{
					// letter = N
					inPlay--;
					for(int i = 0; i < QUEUE_SIZE; i++)
					{
						if(container[i] == in_active)
							Terminated.push_back(container[i]);
					}
					
					in_active = NULL;
				}
			}
			else
			{
				in_active->History[in_active->sub].value--;
			}
		}
		// Wait time
		for(unsigned long int j = 0; j < Input.size(); j++)
		{
			Input[j]->iTimer+=1;
		}
		if(out_active == NULL)
		{
				// search Ready queue 
			if(Output.empty())
			{
				// perhaps statistics?
			}
			else
			{
				out_active = Output.front();
				Output.pop_front();
			}
	
		}
		// out operation not null
		if(out_active != NULL)
		{
			out_active->oActive++;
			if(((out_active->History[out_active->sub].value - 1) == 0))
			{
				out_active->oCount++;
				out_active->History[out_active->sub].value--;
				//switch to next
				out_active->sub++;
				if(out_active->History[out_active->sub].letter == 'C')
				{
					Ready.push_back(out_active);
					out_active = NULL;
				}
				else
				{
					// letter = N
					inPlay--;
					for(int i = 0; i < QUEUE_SIZE; i++)
					{
						if(container[i] == out_active)
							Terminated.push_back(container[i]);
					}
					out_active = NULL;
				}
			}
			else
			{
				//cpu_active->CPUTimer--;
				out_active->History[out_active->sub].value--;
			}
		}
		for(unsigned long int j = 0; j < Output.size(); j++)
		{
			Output[(int)j]->oTimer+=1;
		}

		if((((Entry.empty())&&(Ready.empty())&&(Input.empty())&&(Output.empty())))&&(cpu_active == NULL)&&(in_active == NULL)&&(out_active == NULL))
		{
			cout << "here" << endl;
			break;
		}
		// idle
		if(CPUwork == false)
		{
			idleCnt++;
			cout << "Idle at time " << Timer << " Active processes: ";

			cout <<(in_active == NULL ? 0 : in_active->processID + 101) << " " ;
			cout << (out_active == NULL ? 0 : out_active->processID + 101);
			cout << " " << "\n\n";
		}
		CPUwork = false;
		// initiate active process		
		Timer++;
	}while(Timer < MAX_TIME || (((Entry.empty())&&(Ready.empty())&&(Input.empty())&&(Output.empty()))));
	// clear init array
	cout << "The run has ended" << endl;
	cout << "The final value of the timer was: " << Timer << endl;
	cout << "The amount of time spent idle was: " << idleCnt << endl;
	cout << "Number of terminated processes = " << Terminated.size() << endl;
	cout << "Average waiting time for all terminated processes = ";
	for(long unsigned int i = 0; i < Terminated.size(); i++)
	{
		waitTime += container[(int)i]->CPUTimer + container[(int)i]->oTimer +container[(int)i]->iTimer;
	}
	waitTime/=Terminated.size();
	cout << waitTime << endl;
	for(int i = 0; i < QUEUE_SIZE; ++i)
	{
		delete container[i];
	}


	
    return 0;
}
