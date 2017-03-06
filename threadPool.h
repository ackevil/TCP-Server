/*
 * pthreadPool.h
 *
 *   线程池
 *  Created on: 2017年3月3日
 *      Author: ackevil
 */

#ifndef THREAD_POOP_H
#define THREAD_POOP_H

/*线程结点*/
typedef struct queue_node{
	pthread_t threadId;
	struct queue_node* next;
}thread_queue_node;

/*任务结点*/
typedef struct worker_node{
	void* (*process)(void *arg);
	void* arg;
	struct worker_node* next;
}thread_worker_node;

//函数指针
typedef void* (*Funp)(void *arg);

/*线程池*/
typedef struct {
	int nThreadNum;
	int usedThreadNum;
	int idleThreadNum;
	int curTaskNum;
	pthread_mutex_t queue_lock;
	pthread_mutex_t remove_queue_lock;
	pthread_cond_t task_cond;
	thread_queue_node* threadQueueHead;
		thread_worker_node* workHead;
}thread_pool;


	void initThreadPool();		//初始化线程持
	void addThreadQueueNode(pthread_t threadId); //添加线程到线程队列
	void addWorkerQueueNode( Funp process,void* arg); //添加任务到任务队列
	void* threadTask(void *arg);															//线程函数
	int removeThreadQueueNode(pthread_t threadId);		  //删除线程
#endif




