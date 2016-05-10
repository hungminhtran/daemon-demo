#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h> 
#include <errno.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

int debug = 0;
int counter = 0;
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

void buf_read_callback(struct bufferevent *bev, void *arg)
{
  syslog(LOG_INFO, "read callback %d", counter++);
}

void buf_write_callback(struct bufferevent *bev,  void *arg)
{
  syslog(LOG_INFO, "write callback %d", counter++);
}

void buf_error_callback(struct bufferevent *bev, short what, void *arg)
{
  syslog(LOG_INFO, "error callback %d", counter++);
}

void accept_callback(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *address, int socklen, void *ctx)
{
  struct event_base *base = evconnlistener_get_base(listener);
  struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);

  bufferevent_setcb(bev, buf_read_callback, buf_write_callback, NULL, NULL);

  bufferevent_enable(bev, EV_READ|EV_WRITE);
}

int main(void)
{

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
  // while (1) {
           /* Do some task here ... */
  syslog(LOG_INFO, "still running");

  struct event_base *base;
  struct evconnlistener *listener;
  struct sockaddr_in sin;

  int port = 8000;

  base = event_base_new();
  if (!base) {
    syslog(LOG_INFO, "Couldn't open event base");
    exit(EXIT_FAILURE);
  }
  syslog(LOG_INFO, "create base event successfully");
  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  inet_aton("127.0.0.1", &sin.sin_addr);
  sin.sin_port = htons(port);

  listener = evconnlistener_new_bind(base, accept_callback, NULL, LEV_OPT_CLOSE_ON_FREE, -1, (struct sockaddr*)&sin, sizeof(sin));
  syslog(LOG_INFO, "binding successfully");
  if (!listener) {
    syslog(LOG_INFO, "Couldn't create listener");
    exit(EXIT_FAILURE);
  }
  // evconnlistener_set_error_cb(listener, accept_error_cb);
  syslog(LOG_INFO, "enter dispatch");
  event_base_dispatch(base);
  syslog(LOG_INFO, "exit success");
  closelog();
  exit(EXIT_SUCCESS);
  // }
}
