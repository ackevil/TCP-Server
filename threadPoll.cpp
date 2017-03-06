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


using namespace std;

/*全局变量*/

thread_pool *pool=NULL;

#define THREADNUM 30;


void initThreadPool(){
	pool=(thread_pool*)malloc(sizeof(thread_pool));
	pool->nThreadNum=THREADNUM;
	pool->curTaskNum=0;
	pthread_mutex_init(&pool->remove_queue_lock,NULL);
	pthread_mutex_init(&pool->queue_lock,NULL);
	pthread_cond_init(&pool->task_cond,NULL);
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

	for(int i=0;i<pool->nThreadNum;i++){
		pthread_t threadId;
		pthread_create(&threadId,&attr,&threadTask,NULL);
		addThreadQueueNode(threadId);
	}
	pthread_attr_destroy(&attr);
}

void addThreadQueueNode(pthread_t threadId){
	pthread_mutex_lock(&pool->remove_queue_lock);
	thread_queue_node* node=(thread_queue_node*)malloc(sizeof(thread_queue_node));
	if(node==NULL){
		cerr<<"malloc queue_node failed "<<endl;
		pthread_mutex_unlock(&pool->remove_queue_lock);
		return ;
	}
	node->threadId=threadId;
	node->next=NULL;

	if(pool->threadQueueHead==NULL){
		pool->threadQueueHead=node;
		pthread_mutex_unlock(&pool->remove_queue_lock);
		return ;
	}
	node->next=pool->threadQueueHead->next;
	pool->threadQueueHead->next=node;
	pthread_mutex_unlock(&pool->remove_queue_lock);
	cout<<"创建线程成功"<<endl;
}

int removeThreadQueueNode(pthread_t threadId){
	pthread_mutex_lock(&pool->remove_queue_lock);
	if(pool->threadQueueHead==NULL){
		cerr<<"removeThreadQueueNode failed ,no thread"<<endl;
		pthread_mutex_unlock(&pool->remove_queue_lock);
		return -1;
	}
	thread_queue_node* pre=pool->threadQueueHead;
	thread_queue_node* cur=pool->threadQueueHead;
	int i=1;
	while(i<=pool->nThreadNum&&cur!=NULL){
		if(cur->threadId==threadId)
				break;
		pre=cur;
		cur=cur->next;
		i++;
	}
	if(cur==NULL){
		cerr<<"no threadId"<<endl;
				pthread_mutex_unlock(&pool->remove_queue_lock);
				return -1;
	}
	if(cur->threadId==pool->threadQueueHead->threadId){
		pool->threadQueueHead=pool->threadQueueHead->next;
		free(cur);
		pool->nThreadNum--;
		pthread_mutex_unlock(&pool->remove_queue_lock);
		return 0;
	}
	if(cur->next==NULL){
		pre->next=NULL;
		free(cur);
		pool->nThreadNum--;
		pthread_mutex_unlock(&pool->remove_queue_lock);
				return 0;

	}
	pre->next=cur->next;
	free(cur);
	pool->nThreadNum--;
			pthread_mutex_unlock(&pool->remove_queue_lock);
					return 0;

}

void addWorkerQueueNode(Funp process,void* arg){
	thread_worker_node * work=(thread_worker_node*)malloc(sizeof(thread_worker_node));
	work->process=process;
	work->arg=arg;
	work->next=NULL;
	pthread_mutex_lock(&pool->queue_lock);
	thread_worker_node *p=pool->workHead;
	if(p==NULL){
		pool->workHead=work;
	}else{
		while(p->next!=NULL){
			p=p->next;
		}
		p->next=work;
	}
	pool->curTaskNum++;
	pthread_mutex_unlock(&pool->queue_lock);
	pthread_cond_signal(&pool->task_cond);
}

void* threadTask(void *arg){
	while(1){
		cout<<"thread start...."<<endl;
		pthread_mutex_lock(&pool->queue_lock);
		while(pool->curTaskNum==0){
			pthread_cond_wait(&pool->task_cond,&pool->queue_lock);
		}
		pool->curTaskNum--;
		thread_worker_node* work=pool->workHead;
		pool->workHead=pool->workHead->next;
		pthread_mutex_unlock(&pool->queue_lock);
		(*(work->process))(work->arg);
		free(work);
		work=NULL;
	}
	pthread_exit(NULL);
	return((void*)0);
}


