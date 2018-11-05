/* 
 * Phil Tenteromano
 * 9/14/2018
 * Operating Systems
 * Lab 1:
 * Using fork(), exec(), and wait()
 *
 */

#include <unistd.h>
#include <iostream>
#include <sys/wait.h>

using namespace std;

int main(int argc, char *argv[]) {
	
	if(argc < 2) {
		cout<<"Need a bash command to run!"<<endl;
		exit(1);	
	}

	cout<<"Number of Arguments: "<<argc-1<<endl;
	cout<<"Arguments: ";
	for(int i=1; i < argc; i++) {
		cout<<'"'<<argv[i]<<'"'<<" ";
	}
	cout<<endl;

	pid_t pid = fork();

	if (pid < 0) {
		perror("Fork");
	}
	else if (pid == 0) {
		cout<<"Child running: "<<pid<<endl;
		
		int status;
		status = execvp(argv[1], &argv[1]);
		if (status == -1) {
			perror("Execvp");
		}	
	}
	else {
		if (wait(0) == -1) {
			perror("Wait");
		}
		cout<<"Child Process Terminated"<<endl;
		cout<<"Parent PID: "<<pid<<endl;
	}
	return 0;
}


