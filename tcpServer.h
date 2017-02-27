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
using namespace std;
class TcpServer{
	public:
		TcpServer();
		TcpServer(string ip,int port);
		void initConf();
		int run();
		int initSocket();
		int handleRequest();
		void doEpoll();
		void addEvent(int epollfd,int sockfd,int state);
		void modifyEvent(int epollfd,int sockfd,int state);
		void delEvent(int epollfd,int sockfd,int state);
		void handleAccept(int epollfd,int sockfd);
		void doRead(int epollfd,int sockfd);
		void doWrite(int epollfd,int sockfd);
		void setNonBlock(int sockfd);

	private:
		string ip;
		int port;
		int sockfd;
		int newSockfd;
		sockaddr_in serverAddr;
		sockaddr_in clientAddr;
		int epollfd;
		int nepollEvent;

};




#endif /* TCPSERVER_H_ */
