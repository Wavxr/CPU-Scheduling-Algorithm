#include <iostream>
#include <fstream>
using namespace std;
const int MAX_COLUMNS = 6;

//structure for keeping track of final turnaroundtime and waitingtime while in recursion
struct result{
	int* turnaroundTime;
	int* waitingTime;
}r;

//allocating structure for dynamic memory
void allocateResultArrays(int size) {
    r.turnaroundTime = new int[size];
    r.waitingTime = new int[size];
}

//deallocating for preventing memory leaks
void deallocateResultArrays() {
    delete[] r.turnaroundTime;
    delete[] r.waitingTime;
}

//sorting algorithm for the sorted arrival time
void bubbleSort(int process[][MAX_COLUMNS], int size) {
    for (int i = 0; i < size - 1; i++) 
        for (int j = 0; j < size - i - 1; j++) 
            if (process[j][1] > process[j + 1][1]) 
                for (int k = 0; k < MAX_COLUMNS; k++) 
                    swap(process[j][k], process[j + 1][k]);
}

//polymorphism function for printing table and gantt chart into txt via file handling
void avgTime(int process[][MAX_COLUMNS], int size, int tempBurst[], ofstream& outFile){
	int total_wt = 0, total_tat = 0;
	
	outFile << "\n\nProcess\t"<< "AT"<< "  \tBT " << "  \tFT " << "     TAT " << " \tWT\n";
	
	for (int i=0; i<size; i++) { 
    	total_tat += r.turnaroundTime[i];
        total_wt += r.waitingTime[i];  
        outFile<< process[i][0] << "\t" << process[i][1] <<"\t"<< tempBurst[i] << "\t" 
		<< process[i][3] <<"\t" << r.turnaroundTime[i] <<"\t" << r.waitingTime[i] <<endl; 
    } 
 	outFile << "\nAverage turn around time = " << (float)total_tat / size; 
    outFile << "\nAverage waiting time = " << (float)total_wt / size; 
}

//polymorphism function for displaying table and gantt chart into console
void avgTime(int process[][MAX_COLUMNS], int size, int tempBurst[]){
	int total_wt = 0, total_tat = 0;
	
	cout << "Process\t"<< "AT"<< "  \tBT " << "  \tFT " << "     TAT " << " \tWT\n";
	
	for (int i=0; i<size; i++) { 
    	total_tat += r.turnaroundTime[i];
        total_wt += r.waitingTime[i];  
        cout<< process[i][0] << "\t" << process[i][1] <<"\t"<< tempBurst[i] << "\t" << process[i][3] <<"\t" 
		<< r.turnaroundTime[i] <<"\t" << r.waitingTime[i] <<endl; 
    } 
 	cout << "\nAverage turn around time = " << (float)total_tat / size; 
    cout << "\nAverage waiting time = " << (float)total_wt / size; 
}

//srtf preemptive function that recursively calls itself until all remaining BurstTime is zero
void srtf(int process[][MAX_COLUMNS], int timer, int tempBurst[], int size, int remainingTime[]) {
    int minIndex = -1;
    int minRemainingTime = INT_MAX;

    for (int i = 0; i < size; ++i) {
        if (process[i][1] <= timer && remainingTime[i] > 0 && remainingTime[i] < minRemainingTime) {
            minRemainingTime = remainingTime[i];
            minIndex = i;
        }
    }

    if (minIndex == -1) {
        // If no process can be executed now, find the next arrival time
        int nextArrival = INT_MAX;
        for (int i = 0; i < size; ++i) {
            if (process[i][1] > timer && process[i][1] < nextArrival) {
                nextArrival = process[i][1];
            }
        }
        // If there is a next arrival time, move the timer to that time
        if (nextArrival != INT_MAX) {
            srtf(process, nextArrival, tempBurst, size, remainingTime);
        }
        return;
    }

    int executionTime = min(1, remainingTime[minIndex]);
    process[minIndex][2] -= executionTime;
    timer += executionTime;
    process[minIndex][3] = timer;
    process[minIndex][4] = process[minIndex][3] - process[minIndex][1];
    process[minIndex][5] = process[minIndex][4] - tempBurst[minIndex];

    cout << "Process " << process[minIndex][0] << " executed when time is " << timer << endl;

    if (process[minIndex][2] == 0) {
        r.turnaroundTime[minIndex] = process[minIndex][3] - process[minIndex][1];
        r.waitingTime[minIndex] = process[minIndex][5];
    }
    remainingTime[minIndex] = process[minIndex][2];

    // Sort processes based on remaining burst time
    bubbleSort(process, size);

    srtf(process, timer, tempBurst, size, remainingTime);
}

//priority preemptive function that recursively calls itself until all remaining BurstTime is zero
void prio(int process[][MAX_COLUMNS], int priority[], int timer, int tempBurst[], int size) {
    bool allProcessesDone = true;
    int minPriorityIndex = -1;
    int minPriority = INT_MAX;

    // Find the minimum priority process that has arrived and not yet completed
    for (int i = 0; i < size; ++i) {
        if (process[i][2] > 0 && process[i][1] <= timer) {
            allProcessesDone = false;

            if (priority[i] < minPriority) {
                minPriority = priority[i];
                minPriorityIndex = i;
            }
        }
    }

    // If no process can be executed now, find the next arrival time
    if (minPriorityIndex == -1) {
        int nextArrival = INT_MAX;
        for (int i = 0; i < size; ++i) {
            if (process[i][1] > timer && process[i][1] < nextArrival) {
                nextArrival = process[i][1];
            }
        }
        // If there is a next arrival time, move the timer to that time
        if (nextArrival != INT_MAX) {
            prio(process, priority, nextArrival, tempBurst, size);
        }
        return;
    }

    // Execute the process with the minimum priority
    int executionTime = min(1, process[minPriorityIndex][2]);
    process[minPriorityIndex][2] -= executionTime;
    timer += executionTime;
    process[minPriorityIndex][3] = timer;
    process[minPriorityIndex][4] = process[minPriorityIndex][3] - process[minPriorityIndex][1];
    process[minPriorityIndex][5] = process[minPriorityIndex][4] - tempBurst[minPriorityIndex];

    cout << "Process " << process[minPriorityIndex][0] << " executed when time is " << timer << endl;

    if (process[minPriorityIndex][2] == 0) {
        r.turnaroundTime[minPriorityIndex] = process[minPriorityIndex][4];
        r.waitingTime[minPriorityIndex] = process[minPriorityIndex][5];
    }

    // Continue scheduling remaining processes
    prio(process, priority, timer, tempBurst, size);
}

//round robin preemptive function that recursively calls itself until all remaining BurstTime is zero
void roundrobin(int process[][MAX_COLUMNS], int quantum, int timer, int tempBurst[], int size) {
    bool allProcessesDone = true;

    // Sort processes based on arrival time before scheduling
    bubbleSort(process, size);

    for (int i = 0; i < size; ++i) {
        if (process[i][2] > 0 && process[i][1] <= timer) {  // Check if burst time is remaining and process has arrived
            allProcessesDone = false;

            int executionTime = min(quantum, process[i][2]);
            process[i][2] -= executionTime;  // Reduce burst time
            timer += executionTime;  // Update timer
            process[i][3] = timer;   // Final time
            process[i][4] = process[i][3] - process[i][1];  // Turnaround time
            process[i][5] = process[i][4] - tempBurst[i];  // Waiting time

            cout << "Process " << process[i][0] << " executed from time " << timer - executionTime << " to "<< timer << endl;

            if (process[i][2] == 0) {
                r.turnaroundTime[i] = process[i][4];
                r.waitingTime[i] = process[i][5];
            }
        }
    }

    // Check if any process is remaining and if so, continue scheduling
    if (!allProcessesDone) {
        roundrobin(process, quantum, timer, tempBurst, size);
    }
    // Reset timer if no process is currently running
    else {
        int nextArrival = INT_MAX;
        for (int i = 0; i < size; ++i) {
            if (process[i][2] > 0 && process[i][1] > timer && process[i][1] < nextArrival) {
                nextArrival = process[i][1];
            }
        }
        // If there is a next arrival time, move the timer to that time
        if (nextArrival != INT_MAX) {
            roundrobin(process, quantum, nextArrival, tempBurst, size);
        }
        return;
    }
}
    
int main(){
	int choice;
	while(1){
	system("cls");
	cout << "PREEMPTIVE" << endl;
	cout << "[1] - SHORTEST REMAINING TIME FIRST" << endl;
	cout << "[2] - PRIORITY SCHEDULING" << endl;
	cout << "[3] - ROUND ROBIN" << endl;
	cout << "[4] - EXIT" << endl;
	cout << "Choice(1-4): ";
	cin >> choice;
	
		switch(choice){
			case 1:{
				system("cls");
				int size, elem, timer = 0;
				cout << "SHORTEST REMAINING TIME FIRST" << endl;
				cout << "How many processes: ";
				cin >> size;
				int process[size][MAX_COLUMNS], tempBurst[size], remainingTime[size];
				
				cout << "Enter arrival time (eg. 0 2 4 6..): ";
				for (int i = 0; i < size; i++) { 
					cin >> elem;
					process[i][0] = i+1;	
        			process[i][1] = elem;
    			}
				cout << "Enter burst time (eg. 0 2 4 6.. ): ";
				for (int i = 0; i < size; i++) { 
					cin >> elem;	
        			process[i][2] = elem;
    			}
				allocateResultArrays(size);
				for (int j = 0; j < size; j++)
			    tempBurst[j] = process[j][2];
			    for (int i = 0; i < size; ++i)
        		remainingTime[i] = process[i][2];
			    
			    cout << endl;
			    cout << "============= GANTT CHART =============" << endl;
				srtf(process, timer, tempBurst, size, remainingTime);
				cout << "\n=================  TABLE  =================" << endl;
				avgTime(process, size, tempBurst);
				ofstream srtfOut("srtf_output.txt", ios::app);
            	avgTime(process, size, tempBurst, srtfOut);
				deallocateResultArrays();
				srtfOut.close();
				cout << "\n\n";
				system("pause");
				break;
			}
			case 2:{
				system("cls");
				int size, elem, timer = 0;
				cout << "PRIORITY SCHEDULING PREEMPTIVE" << endl;
				cout << "How many processes: ";
				cin >> size;
				int process[size][MAX_COLUMNS], priority[size], tempBurst[size];
				
				cout << "Enter arrival time (eg. 0 2 4 6..): ";
				for (int i = 0; i < size; i++) { 
					cin >> elem;
					process[i][0] = i+1;	
        			process[i][1] = elem;
    			}
				cout << "Enter burst time (eg. 0 2 4 6.. ): ";
				for (int i = 0; i < size; i++) { 
					cin >> elem;	
        			process[i][2] = elem;
    			}
				cout << "Enter priorities (eg. 0 2 4 6.. ): ";
				for (int i = 0; i < size; i++) { 
					cin >> elem;	
        			priority[i] = elem;
    			}
				allocateResultArrays(size);

				for (int j = 0; j < size; j++)
			    tempBurst[j] = process[j][2];
			    
			    cout << endl;
			    cout << "============= GANTT CHART =============" << endl;
				prio(process, priority, timer, tempBurst, size);
				cout << "\n=================  TABLE  =================" << endl;
				avgTime(process, size, tempBurst);
				ofstream srtfOut("prio_output.txt", ios::app);
            	avgTime(process, size, tempBurst, srtfOut);
				deallocateResultArrays();
				srtfOut.close();
				cout << "\n\n";
				system("pause");
				break;
			}
			case 3:{
				system("cls");
				int size, elem, timer = 0, timeQuantum;
				cout << "ROUND ROBIN PREEMPTIVE" << endl;
				cout << "How many processes: ";
				cin >> size;
				int process[size][MAX_COLUMNS], tempBurst[size];
				
				cout << "Enter arrival time (eg. 0 2 4 6..): ";
				for (int i = 0; i < size; i++) { 
					cin >> elem;
					process[i][0] = i+1;	
        			process[i][1] = elem;
    			}
				cout << "Enter burst time (eg. 0 2 4 6.. ): ";
				for (int i = 0; i < size; i++) { 
					cin >> elem;	
        			process[i][2] = elem;
    			}
				cout << "Enter Time Quantum: ";
				cin >> timeQuantum;
				allocateResultArrays(size);

				for (int j = 0; j < size; j++)
			    tempBurst[j] = process[j][2];
			    
			    cout << endl;
			    bubbleSort(process, size);

	            cout << "\n============= GANTT CHART =============" << endl;
	            roundrobin(process, timeQuantum, timer, tempBurst, size);
	            cout << "\n=================  TABLE  =================" << endl;
	            avgTime(process, size, tempBurst);
				ofstream srtfOut("rr_output.txt", ios::app);
            	avgTime(process, size, tempBurst, srtfOut);
				deallocateResultArrays();
				srtfOut.close();
				cout << "\n\n";
				system("pause");
				break;
				}
				case 4:{
					cout << "EXITING..";
					exit(0);
					break;
				}
				default:{
					cout << "INVALID INPUT! Enter 1-4 only..\n";
					system("pause");
					break;
			}
		}	
	}
	return 0;
}
