/* 
 * Testing program from OS written HW_3
 *
 */


#include <unistd.h>
#include <iostream>
#include <sys/wait.h>

using namespace std;

int main()
{
  pid_t pid, mypid;

  pid = fork();
  if (pid<0) {
    printf("Error, fork failed\n");
    exit(0);
  }
  else if (pid==0) {
    mypid = getpid();
    printf("Child process pid is %d\n",mypid);
    sleep(10);
    exit(0);
  } else {
    mypid=getpid();
    wait(NULL);
    printf("Parent process pid is %d\n",mypid);
  }
}

