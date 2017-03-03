/*
 * pthreadPool.h
 *
 *   线程池
 *  Created on: 2017年3月3日
 *      Author: ackevil
 */

#ifndef THREAD_POOP_H
#define THREAD_POOP_H

typedef struct queue_node{
	pthread_t threadId;
	struct queue_node* next;
}thread_queue_node;

typedef struct worker_node{
	void* (*process)(void *arg);
	void* arg;
	struct worker_node* next;
}thread_worker_node;
typedef void* (*Funp)(void *arg);
void *threadTask(void *arg);


class ThreadPool{
public:
	ThreadPool();
	~ThreadPool();
	void initThreadPool();
	void addThreadQueueNode(pthread_t threadId);
	void addWorkerQueueNode( Funp process,void* arg);

private:
	int nThreadNum;
	int usedThreadNum;
	int idleThreadNum;
	pthread_mutex_t  remove_queue_lock;
	thread_queue_node* threadQueueHead;
	thread_worker_node* workHead;
};
#endif




