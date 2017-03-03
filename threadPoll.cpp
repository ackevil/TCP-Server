/*
 * threadPoll.cpp
 *
 *  Created on: 2017年3月3日
 *      Author: ackevil
 */
#include <iostream>
#include <malloc.h>
#include <cstdlib>
#include "threadPool.h"

void *threadTask(void *arg){
    return((void*)0);
}


using namespace std;

ThreadPool::ThreadPool(){
	threadQueueHead=NULL;
	workHead=NULL;
	initThreadPool();
}
ThreadPool::~ThreadPool(){
	if(threadQueueHead!=NULL){
		thread_queue_node *p=threadQueueHead;
		thread_queue_node *q=NULL;
		while(p){
			q=p;
			p=p->next;
			free(q);
		}
		threadQueueHead=NULL;
	}
	if(workHead!=NULL){
		thread_worker_node *p=workHead;
		thread_worker_node *q=NULL;
			while(p){
				q=p;
				p=p->next;
				free(q);
			}
			workHead=NULL;
		}
}

void ThreadPool::initThreadPool(){
	nThreadNum=30;
	pthread_mutex_init(&remove_queue_lock,NULL);
	pthread_attr_t attr;
	int err;
	err=pthread_attr_init(&attr);
	if(err!=0){
		cerr<<"pthread_attr_init error"<<endl;
		exit(-1);
	}
	err=pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
	if(err!=0){
			cerr<<"pthread_attr_setdetachstate error"<<endl;
			exit(-1);
		}

	for(int i=0;i<nThreadNum;i++){
		pthread_t threadId;
		pthread_create(&threadId,&attr,threadTask,NULL);
		addThreadQueueNode(threadId);
	}
	pthread_attr_destroy(&attr);
}
void ThreadPool::addThreadQueueNode(pthread_t threadId){
	pthread_mutex_lock(&remove_queue_lock);
	thread_queue_node* node=(thread_queue_node*)malloc(sizeof(thread_queue_node));
	if(node==NULL){
		cerr<<"malloc queue_node failed "<<endl;
		pthread_mutex_unlock(&remove_queue_lock);
		return ;
	}
	node->threadId=threadId;
	node->next=NULL;

	if(threadQueueHead==NULL){
		threadQueueHead=node;
		pthread_mutex_unlock(&remove_queue_lock);
		return ;
	}
	node->next=threadQueueHead->next;
	threadQueueHead->next=node;
	pthread_mutex_unlock(&remove_queue_lock);
	cout<<"创建线程成功"<<endl;
}



