
#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <string.h>
#include <netinet/in.h>

void usage(const char* proc)
{
	printf("%s [ip] [port]\n",proc);
}
int main(int argc,char* argv[])
{
	if(argc!=3)
	{
		usage(argv[0]);
		return 1;
	}

	while(1)
	{
	int sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock < 0)
	{
		perror("sock");
		return 1;
	}


	int port = atoi(argv[2]);
	const char* ip = argv[1];

	struct sockaddr_in remote;
	remote.sin_family = AF_INET;
	remote.sin_port = htons(port);
	remote.sin_addr.s_addr = inet_addr(ip);


	if(connect(sock,(struct sockaddr*)&remote,sizeof(remote) ) < 0 )
	{
		perror("connect");
		return 0;
	//	continue;
	}


	char buf[1024];

		memset(buf,'\0',sizeof(buf));
		printf("enter\n");
		fflush(stdout);
		gets(buf);
		write(sock,buf,strlen(buf));
		close(sock);
	}
	return 0;
}

