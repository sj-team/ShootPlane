#include "server.h"

int main(int argc, char *argv[])
{

    create_daemon();    

    parseCMD(argc, argv, false);

	signal(SIGPIPE,SIG_IGN);
	signal(SIGCHLD, SIG_IGN);

    Server * server = new Server();

    server->run();

	return 0;
}
