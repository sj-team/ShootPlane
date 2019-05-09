#include "common.h"

struct sockaddr_in server_addr;
int port;


void create_daemon()
{
	pid_t pid = fork();
	if(pid==-1)
		exit(-1);
	if(pid>0)
		exit(0);

	if(setsid()==-1)
		exit(-1);
	chdir("./");
	umask(0);
	pid = fork();
	if(pid ==-1)
		exit(-1);
	if(pid>0)
		exit(0);

	signal(SIGCHLD, SIG_IGN); 
	
	return ;
}

void myExit()
{
	cerr << strerror(errno) << endl;
	exit(EXIT_FAILURE);
}

void setReusePort(int fd )
{
	int enable = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == -1)
		myExit();
}

void setTime(timeval &timeout, int sec, int usec )
{
	timeout.tv_sec = sec;
	timeout.tv_usec = usec;
	return;
}

void setNonBlock(int fd)
{
	int flag;
	if ((flag = fcntl(fd, F_GETFL, 0)) == -1)
		myExit();
	if (fcntl(fd, F_SETFL, flag | O_NONBLOCK) == -1)
		myExit();

}


int parseCMD(int argc, char **argv ,int isClient )
{
    port = -1;

    for (int i = 1; i<argc ; i++) {
        if (strstr(argv[i], "ip")) {
            server_addr.sin_addr.s_addr = inet_addr(argv[i + 1]);
            i++;
        }

        else if (strstr(argv[i], "port")) {
            if(i>=argc-1){
                cerr << "please input port !\n";
                exit(-1);
            }
            port = atoi(argv[i + 1]);
            i++;
        }

        else {
            cerr<<"input error !\n";
            exit(-1);
        }

    }

    if(port == -1) {
        cerr << "please input port !\n";
        exit(-1);
    }

    server_addr.sin_port = htons(port);
    server_addr.sin_family=AF_INET;
    return true;
}


