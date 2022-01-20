#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>

#include<pthread.h>

//定义生产者和消费者挂起队列
pthread_mutex_t lock;
pthread_cond_t not_full;
pthread_cond_t not_empty;

typedef struct{
	void *(*jobs)(void *);
	void *arg;
}task_t;

typedef struct{
	int queue_cur;
	int queue_max;
	int queue_front;
	int queue_rear;
	task_t *queue_list;
}container_t;


container_t *create_container(int qmax){
	container_t *ct = NULL;
	ct->queue_list = NULL;
	pthread_mutex_init(&lock,NULL);
	pthread_cond_init(&not_full,NULL);
	pthread_cond_init(&not_empty,NULL);
	
	ct = (container_t *)malloc(sizeof(container_t));
	if(ct == NULL)
		perror("ct malloc error");
	ct->queue_cur = 0;
	ct->queue_front = 0;
	ct->queue_rear = 0;
	ct->queue_max = qmax;
	ct->queue_list = (task_t *)malloc(qmax * sizeof(task_t));

	if(ct->queue_list == NULL)
		perror("list malloc error");

	return ct;
}


void *bussness(void *arg){
	//执行工作
	printf("[0x%x]正在执行工作中\n",(unsigned int)pthread_self());
}

int produce(container_t *ct,task_t job){
	pthread_mutex_lock(&lock);
	//生产者执行自己的任务
	if(ct->queue_cur == ct->queue_max)
		pthread_cond_wait(&not_full,&lock);
	printf("生产者[0x%x]正在生产任务\n",(unsigned int)pthread_self());
	ct->queue_list[ct->queue_front].jobs = job.jobs;
	ct->queue_list[ct->queue_front].arg = job.arg;
	(ct->queue_cur)++;
	ct->queue_front = (ct->queue_front + 1)%(ct->queue_max);
	printf("ct->front %d\n",ct->queue_front);
	pthread_mutex_unlock(&lock);
	//唤醒一个消费者
	pthread_cond_signal(&not_empty);

	return 0;
}

void *customer(void *arg){
	pthread_detach(pthread_self());
	//贪婪获取任务
	container_t *ct = (container_t *)arg;
	task_t tmp;

	while(1){
		pthread_mutex_lock(&lock);
		if(ct->queue_cur == 0){
			printf("没有任务，消费者0x%x正在等待\n",(unsigned int)pthread_self());
			pthread_cond_wait(&not_empty,&lock);
		}
		printf("消费者[0x%x]获取任务成功\n",(unsigned int)pthread_self());
		tmp.jobs = ct->queue_list[ct->queue_rear].jobs;
		tmp.arg = ct->queue_list[ct->queue_rear].arg;
		(ct->queue_cur)--;
		ct->queue_rear = (ct->queue_rear + 1)%(ct->queue_max);
		pthread_mutex_unlock(&lock);
		pthread_cond_signal(&not_full);
		//执行任务在解锁之后
		(tmp.jobs)(tmp.arg);
	}
	pthread_exit(NULL);
}


int main(){
	container_t *ct = NULL;
	ct = create_container(100);
	pthread_t pid;
	for(int i =0;i<5;i++){
		pthread_create(&pid,NULL,customer,(void *)ct);
	}
	task_t tmp;
	tmp.jobs = bussness;
	tmp.arg = NULL;
	for(int i = 0;i<20;i++){
		produce(ct,tmp);
	}
	while(1)
		sleep(1);
	return 0;
}
