
#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <string.h>
#include <netinet/in.h>

#define _FD_NUM_  512
#define _FD_DEFAULT_VAL_  -1

typedef struct _select_fd
{
	int max_fd;
	int fd_arr[512];
}select_fd_t;

void usage(const char* proc)
{
	printf("%s [ip] [port]\n",proc);
}

static void add_read_fd(fd_set* r_set,select_fd_t* select_set) //将参数2 的值添加到参数1中
{
	int i = 0;
	select_set->max_fd = _FD_DEFAULT_VAL_;
	for(;i<_FD_NUM_;++i)
	{
		if (select_set->fd_arr[i] != _FD_DEFAULT_VAL_)
		{
			FD_SET(select_set->fd_arr[i],r_set);
			if(select_set->fd_arr[i] , select_set->max_fd)
			{
				select_set->max_fd =  select_set->fd_arr[i];
			}
		}
	}
}

							

static int add_new_fd(select_fd_t *select_fd,int new_fd)//保存到数组
{
	int i = 0;
	for(;i<_FD_NUM_;++i)
	{
		if(select_fd->fd_arr[i] == _FD_DEFAULT_VAL_)//no use
		{
			select_fd->fd_arr[i]=new_fd;
			return 0; //yes
		}
	}
	return 1; //failed
}



static int delete_new_fd(select_fd_t *select_fd,int fd)
{
	int i=1;
	for(;i<_FD_NUM_;++i)
	{
		int curr_fd = select_fd->fd_arr[i];
		if(curr_fd == fd)
		{
			select_fd->fd_arr[i] = _FD_DEFAULT_VAL_;
		}
	}
}

static int init_select_set(select_fd_t* select_set,int listen_fd)
{
	select_set->max_fd =listen_fd;
	select_set->fd_arr[0]=listen_fd;

	int i=1;
	for(;i<_FD_NUM_;++i)
	{
		select_set->fd_arr[i]=_FD_DEFAULT_VAL_;
	}
}


static int read_data_show(int new_fd)
{
	char buf[1024];

	ssize_t sz = 0;
	int total=0;
	while ( (sz = read(new_fd,buf+total,64) )>0 )
	{
		total+=sz;
	}
	if( 0 == sz)//read end
	{
		buf[total]='\0';
		printf("client data: %s\n",buf);
		return 0;//read end ,return 0
	}
	else if(sz < 0)
	{
		perror("read");
		return -1;
	}
	else
	{
		//do nothing
	}
}


int start(const char* ip,short port)
{
	int sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock < 0)
	{
		perror("socket");
		exit(1);
	}

	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(port);
	local.sin_addr.s_addr = inet_addr(ip);

	if ( bind(sock,(struct sockaddr*)&local,sizeof(local)) < 0 )
	{
		perror("bind");
		//close(sock);
		exit(1);
	}

	if (listen(sock,5) < 0 )
	{
		perror("listen");
		exit(1);
	}

	return sock;
}

int main(int argc,char* argv[])
{
	if(argc != 3 )
	{
		usage(argv[0]);
		exit(1);
	}


	int port = atoi(argv[2]);
	int listen_sock = start(argv[1],port);

	if(listen_sock < 0)
		exit(2);

	select_fd_t select_set;
	init_select_set(&select_set,listen_sock);
	fd_set r_set;//read file fd set

	while(1)
	{
		FD_ZERO(&r_set);
		add_read_fd(&r_set,&select_set); //将参数2 的值添加到参数1中
		struct timeval timeout ={3,500};//fei zuse
		 switch (select(select_set.max_fd+1,&r_set,NULL,NULL,&timeout) )
		 {
			case 0:
				perror("timeout");
				break;
			case -1:
				perror("select");
				break;
			default://success
				{ 
					int i = 0;
					for(;i<_FD_NUM_;++i)
					{
						int fd = select_set.fd_arr[i];
						if(0==i && FD_ISSET(fd,&r_set) )//new connect
						{
							struct sockaddr_in client;
							socklen_t len = sizeof(client);
							int new_fd = accept(fd,(struct sockaddr_in*)&client,&len);

							if(new_fd == -1)
							{
								perror("accept");
								continue;//失败，继续下次循环
							}

							if ( 0 == add_new_fd(&select_set,new_fd) )//add yes
							{
								//do nothing
							}
							else//add failed,array is full
							{
								close(fd);  //资源不足，暂时请求失败
							}
							continue;
						}
						//不是监听sock
						if( fd != _FD_DEFAULT_VAL_ && FD_ISSET(fd,&r_set) )
						{//read 就绪
							if( 0 == read_data_show(fd) )
							{
								delete_new_fd(&select_set,fd);
								close(fd);
							}
							else
							{
								//do nothing
							}
						}
					}
					
				}
				break;
						
		 }

	}

	return 0;
}






