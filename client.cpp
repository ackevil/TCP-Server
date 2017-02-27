/*
 * client.cpp
 *
 *  Created on: 2017年2月27日
 *      Author: ackevil
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <string.h>
#include <sys/select.h>

#define BUFFER_LEN 1024
#define BACKLOG    5
#define MAXCONNUM  1024
#define SERV_PORT  8080

int main()
{
	int i=0;
	int n=0;
	char buf[BUFFER_LEN];
	int client_fd;
	struct sockaddr_in serv;
	//client_fd = socket(AF_INET,SOCK_STREAM,0);
//	for(i=0;i<10;i++)
	int num=1;
	while(num<=10000)
	{
		client_fd = socket(AF_INET,SOCK_STREAM,0);
		memset(&serv,0,sizeof(serv));
		serv.sin_family = AF_INET;
		serv.sin_addr.s_addr = htonl(INADDR_ANY);
		serv.sin_port = htons (SERV_PORT);
		int err =connect( client_fd ,(struct sockaddr*)(&serv),sizeof(serv));
		memset(buf,0,BUFFER_LEN);
		strcpy(buf,"hello,world!");
		send(client_fd,buf,strlen(buf),0);
		memset(buf,0,BUFFER_LEN);
		n=recv(client_fd,buf,BUFFER_LEN,0);
		if(n>0)
		{
			printf("%s\n",buf);
			printf("the connect of  %d\n",num);
			close(client_fd);
			usleep(1);
		}
		num++;
	}
	return 0;
}




