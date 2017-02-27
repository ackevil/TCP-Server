/*
 * main.cpp
 *
 *  Created on: 2017年2月27日
 *      Author: ackevil
 */
#include <iostream>
#include <string>
#include <unistd.h>
#include <cstdlib>
#include "tcpServer.h"
using namespace std;
int main(int argc,char* argv[]){
	int opt=0;
	string ip="127.0.0.1";
	int port=getuid()==0?80:8080;
	while((opt=getopt(argc,argv,"a::p::"))!=-1){
		switch(opt){
			case 'a':
				ip=optarg;
				break;
			case 'p':
				port=atoi(optarg);
				break;
		}
	}
	TcpServer server(ip,port);
	server.run();
	return 0;
}



