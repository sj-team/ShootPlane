#include <iostream>
#include <fstream>
#include <stdio.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <cstdlib>
#include <fcntl.h>
#include <net/if.h>
#include <signal.h>
#include <netdb.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>

using namespace std;

extern int errno;

extern struct sockaddr_in server_addr;
extern int port;
extern const char * orderMsg [];

void create_daemon();

void myExit();

void setReusePort(int fd);

void setNonBlock(int fd);

// bool writeFile(const ClientInfo & cinfo, bool);

void setTime(timeval &timeout, int sec, int usec) ;

int parseCMD(int argc, char **argv ,int isClient );