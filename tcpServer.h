/*
 * tcpServer.h
 *
 *  Created on: 2017年2月27日
 *      Author: ackevil
 */

#ifndef TCPSERVER_H_
#define TCPSERVER_H_
#include <string>
#include <netinet/in.h>
#include "threadPool.h"
using namespace std;

typedef struct {
	int epollfd;
	int fd;
	int flag;
}arginfo;
class TcpServer{
	public:
		TcpServer();
		TcpServer(string ip,int port);
		void initConf();
		int run();
		int initSocket();
		int handleRequest();
		void doEpoll();
		static void addEvent(int epollfd,int sockfd,int state);
		static void modifyEvent(int epollfd,int sockfd,int state);
		static void delEvent(int epollfd,int sockfd,int state);
		static void handleAccept(int epollfd,int sockfd);
		static void* process(void *arg);
		static void doRead(int epollfd,int sockfd);
		static void doWrite(int epollfd,int sockfd);
		static void setNonBlock(int sockfd);

	private:
		string ip;
		int port;
		int sockfd;
		int newSockfd;
		sockaddr_in serverAddr;
		sockaddr_in clientAddr;
		int epollfd;
		int nepollEvent;
		/**threadPool**/
};




#endif /* TCPSERVER_H_ */
