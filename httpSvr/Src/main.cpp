#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "log.h"
#include "httpserver.h"

int main(int argc, char **argv)
{
	if(argc < 3)
	{
		errorf("Usage: ./%s ip port\n", argv[0]);
		return -1;
	}
	std::string ip(argv[1]);
	int port = atoi(argv[2]);
	Http::HttpServer httpserver(ip, port);

	if(httpserver.start(1024) < 0)
	{
		errsys("server create failed\n");
		return -1;
	}

	printf("************************************\n");
	printf("* List of classes of commands:\n\n");
	printf("* CTRL^D -- exit\n");
	printf("************************************\n");
	
	for(;;)
	{
		char buff[1024];
		char *cmd = fgets(buff, sizeof(buff), stdin);
		if(cmd == NULL)
		{
			break;
		}
	}

	tracef("terminate ...\n");
	return 0;
}
