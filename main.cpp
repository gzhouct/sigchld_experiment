#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

/*************************************************************************
 *      Signal handler: To take care of <defunct> process
 ************************************************************************/
/* appSignalInstall
 * Purpose:  install signal handler
 * Params:   None
 * Return:   None
 */
void appSignalInstall( unsigned int signo,  void (*functionp)(int, siginfo_t *, void *) )
{
    struct sigaction sa;

    if (functionp == NULL)
        return;
    sa.sa_sigaction = functionp;
    sigemptyset(&sa.sa_mask);

    sa.sa_flags = SA_NODEFER | SA_SIGINFO;

    if (sigaction(signo, &sa, NULL) < 0)
    {
        fprintf(stderr,"Signal handling falied");
    }
}

/* appSIGCHLDhandler
 * Purpose:  appSIGCHLDhandler
 * Params:   None
 * Return:   None
 */
void appSIGCHLDhandler (int signo, siginfo_t *siginfop, void *contextp)
{
    int status;

    /* Allow the SDK process to terminate */
    pid_t pid = wait(&status);
    printf("Signal %d triggered by child pid: %d. current pid: %d from pid: %d\n", signo, pid, getpid(), siginfop?siginfop->si_pid:-1);
    /* Assume that the above wait() consumes the kill signal from elsewhere out of our application */
    pid = wait(&status);
    printf("Signal %d triggered by child pid: %d. current pid: %d from pid: %d\n", signo, pid, getpid(), siginfop?siginfop->si_pid:-1);
}

/* SignalInit
 * Purpose:  initialize signal handler
 * Params:   None
 * Return:   None
 */
void SignalInit()
{
   appSignalInstall (SIGCHLD, appSIGCHLDhandler);
}

#define   BUF_SIZE   100

void  ChildProcess(char [], char [], int);    /* child process prototype  */

int main(void)
{
    /* Initialize signal handler */
    SignalInit();

     pid_t   pid1, pid2, pid;
     int     status;
     int     i;
     char    buf[BUF_SIZE];

     printf("*** Parent is about to fork process 1 ***\n");
     if ((pid1 = fork()) < 0) {
          printf("Failed to fork process 1\n");
          exit(1);
     }
     else if (pid1 == 0) 
          ChildProcess("First", "   ", 50);

     printf("*** Parent is about to fork process 2 ***\n");
     if ((pid2 = fork()) < 0) {
          printf("Failed to fork process 2\n");
          exit(1);
     }
     else if (pid2 == 0) 
          ChildProcess("Second", "      ", 300);

     /*
     sprintf(buf, "*** Parent enters waiting status .....\n");
     write(1, buf, strlen(buf));
     pid = wait(&status);
     sprintf(buf, "*** Parent detects process %d was done ***\n", pid);
     write(1, buf, strlen(buf));
     pid = wait(&status);
     printf("*** Parent detects process %d is done ***\n", pid);
     printf("*** Parent exits ***\n");
     */
     while(1);
     return 0;
}

void  ChildProcess(char *number, char *space, int numcount)
{
     pid_t  pid;
     int    i;
     char   buf[BUF_SIZE];

     pid = getpid();
     sprintf(buf, "%s%s child process starts (pid = %d)\n", 
             space, number, pid);
     write(1, buf, strlen(buf));
     for (i = 1; i <= numcount; i++) {
          sprintf(buf, "%s%s child's output, value = %d\n", space, number, i); 
          write(1, buf, strlen(buf));
          usleep(100000);
     }
     sprintf(buf, "%s%s child (pid = %d) is about to exit\n", 
             space, number, pid);
     write(1, buf, strlen(buf));     
     exit(0);
}