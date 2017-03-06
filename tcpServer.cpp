/*
 * tcpServer.cpp
 *
 *  Created on: 2017年2月27日
 *      Author: ackevil
 */
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cerrno>
#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <cstdlib>
#include <cstring>
#include "tcpServer.h"

using namespace std;

TcpServer::TcpServer():ip("127.0.0.1"),port(8080){
	initConf();
}

TcpServer::TcpServer(string ip,int port):ip(ip),port(port){
	initConf();
}
int TcpServer::initSocket(){
	string funcName="initSocket: ";
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0){
		cerr<<funcName<<"Failed create socket"<<endl;
		return -1;
	}
	in_addr_t s;
	inet_pton(AF_INET,ip.c_str(),(void*)&s);
	serverAddr.sin_family=AF_INET;
	serverAddr.sin_port =htons(port);
	serverAddr.sin_addr.s_addr=s;
	//serverAddr.sin_addr.s_addr=INADDR_ANY;
	int reuse=1;
	if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,(void *)&reuse,sizeof(int))<0){
		cerr<<funcName<<"Failed setsocketopt SO_REUSEADDR"<<endl;
		return -1;
	}
	setNonBlock(sockfd);
	if(bind(sockfd,(struct sockaddr*)&serverAddr,sizeof(serverAddr))<0){
		cerr<<funcName<<"Failed bind socket"<<endl;
		return -1;
	}
	if(listen(sockfd,5)<0){
		cerr<<funcName<<"Failed listen socket"<<endl;
		return -1;
	}
	return 0;
}

int TcpServer::run(){
	string funcName="run: ";
	int rv=0;
	if((rv=initSocket())<0){
		cout<<funcName<<"initSocket failed "<<endl;
		return -1;
	}
	//if(epoll==1){
		doEpoll();
//	}
	/*
	while(true){
		unsigned int len;
		if((newSockfd=accept(sockfd,(struct sockaddr*)&clientAddr,&len))<0){
			if(errno==EINTR)
				continue;
			else{
				cerr<<funcName<<" accept 	failed"<<endl;
						return -1;
			}
		}
		if(fork()==0){
			handleRequest();
			exit(0);
		}
		close(newSockfd);
	}
	*/
	return 0;

}
void TcpServer::initConf(){
	nepollEvent=100;
	initThreadPool();
}
void TcpServer::doEpoll(){
	struct epoll_event events[nepollEvent];
	epollfd=epoll_create(1000);
	addEvent(epollfd,sockfd,EPOLLIN);
	int rv;
	int fd;
	while(true){
		rv=epoll_wait(epollfd,events,nepollEvent,-1);
		for(int i=0;i<rv;i++){
			fd=events[i].data.fd;
			if(fd==sockfd&&(events[i].events&EPOLLIN)){
				handleAccept(epollfd,sockfd);
			}else if(events[i].events&EPOLLIN){
				arginfo* arg=(arginfo*)malloc(sizeof(arginfo));
				arg->epollfd=epollfd;
				arg->fd=events[i].data.fd;
				arg->flag=0;  //0 read
				addWorkerQueueNode(process,(void*)(arg));
				//doRead(epollfd,events[i].data.fd);
			}else if (events[i].events & EPOLLOUT){
				arginfo* arg=(arginfo*)malloc(sizeof(arginfo));
								arg->epollfd=epollfd;
								arg->fd=events[i].data.fd;
								arg->flag=1;  //1 write
					addWorkerQueueNode(process,(void*)(arg));
			//	doWrite(epollfd,events[i].data.fd);
			}
		}
	}

}
void* TcpServer::process(void *arg){
	int epollfd=((arginfo*)arg)->epollfd;
	int fd=((arginfo*)arg)->fd;
	int flag=((arginfo*)arg)->flag;
	if(flag==0){
		doRead(epollfd,fd);
	}else{
		doWrite(epollfd,fd);
	}
	return (void*)0;
}
void TcpServer::doWrite(int epollfd,int sockfd){
	const char* buf="HTTP/1.1 200 OK\r\nContent-Type:text/html; charset=UTF-8\r\n\r\nHello World";
	int s=strlen(buf)+1;
	int n=0;
	int nwrite;
	while(s>0){
		nwrite=send(sockfd,buf+n,s-n,MSG_DONTWAIT);
		if(nwrite==-1){
			if(errno==EAGAIN||errno==EWOULDBLOCK)
			{
				break;
			}else{
				delEvent( epollfd, sockfd,EPOLLOUT|EPOLLET);
				close(sockfd);
				return ;
			}
		}
		n+=nwrite;
		s-=nwrite;
	}
	close(sockfd);
	modifyEvent( epollfd, sockfd,EPOLLIN|EPOLLET);

}
void TcpServer::doRead(int epollfd,int sockfd){
	  int n = 0;
	  char buf[4096];
	  memset(buf,0,4096);
	  int nread;
	  while (( nread = read(sockfd, buf + n, 4096-n)) > 0) {
		  	  n += nread;
	       }
	  if (nread == -1) {
		  	if(errno==EAGAIN||errno==EWOULDBLOCK){
		  			cout<<buf<<endl;
		  			modifyEvent( epollfd, sockfd,EPOLLOUT|EPOLLET);
		  	}else{
		  		 delEvent( epollfd, sockfd,EPOLLIN|EPOLLET);
		  		close(sockfd);
		  		 return;
		  	  	 }
	  }else if(nread==0){
		  cout<<"客户端关闭连接"<<endl;
		  close(sockfd);
	 }else{}
	 // modifyEvent( epollfd, sockfd,EPOLLOUT|EPOLLOUT|EPOLLET);

}
void TcpServer::setNonBlock(int sockfd){
	int opts;
	opts=fcntl(sockfd,F_GETFL);
	if(opts<0){
		cerr<<"fcntl get error"<<endl;
		exit(-1);
	}
	if(fcntl(sockfd,F_SETFL,opts|O_NONBLOCK)<0){
		cerr<<"fcntl F_SETFL error"<<endl;
		exit(-1);
	}
}
void TcpServer::handleAccept(int epollfd,int sockfd){
	int newSockfd;
	struct sockaddr_in clientAddr;
	unsigned int len=sizeof(clientAddr);
	char clientIP[100];
	while((newSockfd=accept(sockfd,(struct sockaddr*)&clientAddr,&len))>0){
		if (inet_ntop(AF_INET,&clientAddr.sin_addr.s_addr,clientIP,sizeof(clientIP))==0)
				return ;
			int port=ntohs(clientAddr.sin_port);
			cout<<"来自客户端: "<<clientIP<<":"<<port<<"的连接......"<<endl;
		setNonBlock(newSockfd);
		addEvent(epollfd,newSockfd,EPOLLIN|EPOLLET);
	}
	/*
	if(newSockfd==-1&&(errno==EAGAIN||errno==EWOULDBLOCK)){
		continue;
	}
	*/

}
void TcpServer::modifyEvent(int epollfd,int sockfd,int state){
	struct epoll_event event;
	event.events=state;
	event.data.fd=sockfd;
	epoll_ctl(epollfd,EPOLL_CTL_MOD,sockfd,&event);
}
void TcpServer::delEvent(int epollfd,int sockfd,int state){
	struct epoll_event event;
	event.events=state;
	event.data.fd=sockfd;
	epoll_ctl(epollfd,EPOLL_CTL_DEL,sockfd,&event);
}
void TcpServer::addEvent(int epollfd,int sockfd,int state){
	struct epoll_event event;
	event.events=state;
	event.data.fd=sockfd;
	epoll_ctl(epollfd,EPOLL_CTL_ADD,sockfd,&event);
}
int TcpServer::handleRequest(){
	ssize_t n;
	char buf[4096];
	char clientIP[20];
	char* p=buf;
	if (inet_ntop(AF_INET,&clientAddr.sin_addr.s_addr,clientIP,sizeof(clientIP))==0)
		return -1;
	int port=ntohs(clientAddr.sin_port);
	cout<<"来自客户端: "<<clientIP<<":"<<port<<"的连接......"<<endl;
	while((n=recv(newSockfd,buf,4096,0))>0){
		//int left=n;
		//p=buf;
		//int len;
		//cout<<n<<endl;
		write(newSockfd,buf,n);
		/*
		while(left>0){
			if((len=write(newSockfd,p,left))<=0){

				if(len<0&&errno==EINTR)
									len=0;
				else
					return -1;

			}
			left-=len;
			p=p+len;
		}
*/
	}
	if(n==0)
		close(newSockfd);
	cout<<"connection is closed"<<endl;
	return 0;
}





