#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h> 

#include <event.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory>
#include <cstdint>
#include <iostream>


#define SERVER_PORT 8080
int debug = 0;

struct client {
  int fd;
  struct bufferevent *buf_ev;
};

int setnonblock(int fd)
{
  int flags;

  flags = fcntl(fd, F_GETFL);
  flags |= O_NONBLOCK;
  fcntl(fd, F_SETFL, flags);
}

void buf_read_callback(struct bufferevent *incoming, void *arg)
{
  struct evbuffer *evreturn;
  char *req;

  req = evbuffer_readline(incoming->input);
  if (req == NULL)
    return;

  evreturn = evbuffer_new();
  evbuffer_add_printf(evreturn,"You said %s\n",req);
  bufferevent_write_buffer(incoming,evreturn);
  evbuffer_free(evreturn);
  free(req);
}

void buf_write_callback(struct bufferevent *bev, void *arg)
{

}

void buf_error_callback(struct bufferevent *bev, short what, void *arg)
{
  struct client *client = (struct client *)arg;
  bufferevent_free(client->buf_ev);
  close(client->fd);
  free(client);
}

void accept_callback(int fd, short ev, void *arg)
{
  int client_fd;
  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);
  struct client *client;

  client_fd = accept(fd, (struct sockaddr *)&client_addr, &client_len);
  if (client_fd < 0)
  {
    // ("Client: accept() failed");
    return;
  }

  setnonblock(client_fd);

  client = (struct client*)calloc(1, sizeof(*client));
  if (client == NULL)
    // std::cout <<"malloc failed";
    client->fd = client_fd;

  client->buf_ev = bufferevent_new(client_fd,
   buf_read_callback,
   buf_write_callback,
   buf_error_callback,
   client);

  bufferevent_enable(client->buf_ev, EV_READ);
}

int main(int argc, char **argv)
{
          /* Our process ID and Session ID */

  pid_t pid, sid;

        /* Fork off the parent process */
  pid = fork();
  if (pid < 0) {
    std::cout<<"Fork failure"<<std::endl; 
    exit(EXIT_FAILURE);
  }
        /* If we got a good PID, then
           we can exit the parent process. */
  if (pid > 0) {
    std::cout<<"fork success"<<std::endl; 
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
    syslog(LOG_INFO, "set sid failure");  
    exit(EXIT_FAILURE);
  }



        /* Change the current working directory */
  if ((chdir("/")) < 0) {
                /* Log the failure */
    syslog(LOG_INFO, "change dir failure");  
    exit(EXIT_FAILURE);
  }

        /* Close out the standard file descriptors */
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

        /* Daemon-specific initialization goes here */
        /* The Big Loop */
  while (1) {
           /* Do some task here ... */
    syslog(LOG_INFO, "still running");

    int socketlisten;
    struct sockaddr_in addresslisten;
    struct event accept_event;
    int reuse = 1;

    event_init();

    socketlisten = socket(AF_INET, SOCK_STREAM, 0);

    if (socketlisten < 0)
    {
      syslog(LOG_INFO, "Failed to create listen socket");
      return 1;
    }

    memset(&addresslisten, 0, sizeof(addresslisten));

    addresslisten.sin_family = AF_INET;
    addresslisten.sin_port = htons(8888);
  // addresslisten.sin_addr.s_addr = INADDR_ANY;
    inet_aton("127.0.0.1", &addresslisten.sin_addr);


    if (bind(socketlisten, (struct sockaddr *)&addresslisten, sizeof(addresslisten)) < 0)
    {
      syslog(LOG_INFO, "Fail to bind");
      return 1;
    }

    if (listen(socketlisten, 5) < 0)
    {
      syslog(LOG_INFO, "fail to listen socket");
      return 1;
    }

    setsockopt(socketlisten, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    setnonblock(socketlisten);

    event_set(&accept_event, socketlisten, EV_READ|EV_PERSIST, accept_callback, NULL);

    event_add(&accept_event, NULL);
    syslog(LOG_INFO, "begin dispathc");
    event_dispatch();

    close(socketlisten);
    syslog(LOG_INFO, "exit success");
    closelog();
    exit(EXIT_SUCCESS);
  }
}