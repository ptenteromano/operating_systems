/*
 * Phil Tenteromano
 * 10/11/2018
 * Lab 2 - Round Robin Scheduler
 *
*/

#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <string>
#include <cctype>
#include <list>
#include <iterator>

using namespace std;

const int TIMESLICE=2;
bool alarmFired = false;

/*
 * void sigalarm_handler(int signum)
 * 	User defined signal handler for the signal number that is registered
 * 	using signal(unsigned int signum, sighandler_t)
 * @param signum: signal number of the signal that caused the handler to fire.
 */

// process structure
struct PROCESS {
	string name;
	int cpuBurst;
};

void sigalarm_handler(int signum)
{
    // To simulate a preemptive, timesliced scheduling system, a signal handler
    // must be registered that simulates the CPU timer that fires whenever the
    // system has to schedule another process.
    cout << "Time slice complete " << endl;
    alarmFired = true;
}

/*
 * main(void)
 * 	Demonstrates the use of signal to register a signal handler for a signal
 * 	as well as the use of alarm to set an alarm for TIMESLICE seconds. Then
 * 	the process sleeps for some arbitrary amount of time. The process awakens
 * 	due to the alarm since the sleep time is greater than the alarm time.
 */
int main(void) {
	
	cout << "Input Process Name, followed by a space and then the Integer CPU burst\n"
		 << "Hit enter after each input, and (ctrl-D) when done inputting"
		 << endl;
	
	string line;
	list<PROCESS> rr_sched;
	// parse input into a new PROCESS struct
	while (getline(cin, line)) {
		string pName;
		string pTime;
		bool space = false;
		for (char&c : line) {
			if (isspace(c))
				space = true;
			if (!space) 
				pName += c;
			else
				pTime += c;
		}
		if (!space) 
			cout<<"Invalid Process, try again" << endl;
		else {
			try {
				int i = stoi(pTime, nullptr, 0);
				// create new struct, insert struct into list
				PROCESS p;
				p.name = pName;
				p.cpuBurst = i;
				rr_sched.push_back(p);
				cout << "Process entered, continue input or (ctrl-D) to stop" << endl;
			}
			catch(...) {
				cout << "Invalid time entered" << endl;
				cout << rr_sched.empty() << endl;
			}
		}
	}
	
	signal(SIGALRM, sigalarm_handler);
	list <PROCESS> :: iterator rr;
	int totalTime = 0;

	// round robin loop until every process has completed
    while (!rr_sched.empty()) {	
		cout << "Round Robin circling: " << rr_sched.size() << " processes in queue." << endl;
		// iterate through processes
		for (rr = rr_sched.begin(); rr != rr_sched.end(); ++rr) {
			alarm(TIMESLICE);
			cout << "Processing: " << rr->name << " with a CPU burst of " 
				 << rr->cpuBurst << " remaining" << endl;	
    
			sleep(rr->cpuBurst);

        	if (alarmFired == true) {
            	rr->cpuBurst -= TIMESLICE;
				totalTime += TIMESLICE;
			}
        	else {
				totalTime += rr->cpuBurst;
				cout << "Process: " << rr->name << " Finished" << endl;
				rr = rr_sched.erase(rr);
			}
        	// Reset alarmFired.
        	alarmFired = false;
		}
    }
	cout << "Round Robin complete." << endl
		 << "Total time: " << totalTime << endl;
}



