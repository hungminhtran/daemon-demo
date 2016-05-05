#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h> 

//libEvent
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include <arpa/inet.h>


int main(void) {
    
        /* Our process ID and Session ID */
    pid_t pid, sid;
    
        /* Fork off the parent process */
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
        /* If we got a good PID, then
           we can exit the parent process. */
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

        /* Change the file mode mask */
    umask(0);
    
        /* Open any logs here */
    openlog("demo-daemon-log", LOG_PID|LOG_CONS, LOG_USER);
    syslog(LOG_INFO, "initializing");
    
        /* Create a new SID for the child process */
    sid = setsid();
    if (sid < 0) {
                /* Log the failure */
        exit(EXIT_FAILURE);
    }
    

    
        /* Change the current working directory */
    if ((chdir("/")) < 0) {
                /* Log the failure */
        exit(EXIT_FAILURE);
    }
    
        /* Close out the standard file descriptors */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    
        /* Daemon-specific initialization goes here */
    
        /* The Big Loop */
    int counter = 0;
    while (1) {
           /* Do some task here ... */
      syslog(LOG_INFO, "still running %d", counter);\
      counter++;
      
           sleep(5); /* wait 5 seconds */
  }
  closelog();
  exit(EXIT_SUCCESS);
}
