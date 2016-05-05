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
#include <memory>
#include <cstdint>
#include <iostream>
#include <evhttp.h>


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
        // while (1) {
           /* Do some task here ... */
		syslog(LOG_INFO, "still running %d", counter);\
		counter++;           
                if (!event_init())
          {
            std::cerr << "Failed to init libevent." << std::endl;
            return -1;
          }
          char const SrvAddress[] = "127.0.0.1";
          std::uint16_t SrvPort = 5555;
          std::unique_ptr<evhttp, decltype(&evhttp_free)> Server(evhttp_start(SrvAddress, SrvPort), &evhttp_free);
          if (!Server)
          {
            std::cerr << "Failed to init http server." << std::endl;
            return -1;
          }
          void (*OnReq)(evhttp_request *req, void *) = [] (evhttp_request *req, void *) //????
          {
            auto *OutBuf = evhttp_request_get_output_buffer(req);
            if (!OutBuf)
              return;
            evbuffer_add_printf(OutBuf, "<html><body><center><h1>Hello World!</h1></center></body></html>");
            evhttp_send_reply(req, HTTP_OK, "zas", OutBuf);
          };
          evhttp_set_gencb(Server.get(), OnReq, nullptr);
          if (event_dispatch() == -1)
          {
            std::cerr << "Failed to run messahe loop." << std::endl;
            return -1;
          }
           // sleep(5); /* wait 5 seconds */
        // }
	closelog();
   exit(EXIT_SUCCESS);
}
