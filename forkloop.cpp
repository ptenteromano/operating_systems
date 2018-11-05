#include <iostream>
#include <unistd.h>
#include <sys/types.h>

using namespace std;

int main() {

	for(int i = 1; i < 4; i++) {
		fork();
		cout<<"Process pid: "<<getpid()<<" Inside i: "<<i<<endl;
	}

}

// 2^n processes
// once created, they run randomly
// COUT causes OS interrupt - observing 'changes' behavior
// notice 'i' is not shared
// each process gets its own copy of i
// try output


