#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

int fd; //文件描述符 
int in = 0, out = 0;
char buffer1[1]; //存放生产者从文件中读取的字符
char buffer[3]; //缓冲区，大小为3
int count = 1; //生产者每次从文件中读取一个字符
sem_t empty;
sem_t full;
pthread_mutex_t mutex;

void producer(void *args) {

    //获取从主线程传过来的线程编号
    int i = *((int *)args) + 1;

    //每个生产者进行四次读文件操作
    for (int j = 0; j < 4; j++){
        //当缓冲区不满时读文件内容写入
        sem_wait(&empty);
        //当文件和缓冲区未被访问时，读文件和写入缓冲区
        pthread_mutex_lock(&mutex);

        //每次读取一个字符，写入缓冲区
        read(fd, buffer1, count);
        buffer[in] = buffer1[0];
        //将缓冲区写入位置移位
        in = (in + 1) % 3;
        printf("[tid](%ld)-[produer](%d): write to buffer success(%c)\n", (long)pthread_self(), i, buffer1[0]);

        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }

    printf("[tid](%ld)-[produer](%d): exit\n", (long)pthread_self(), i);
    pthread_exit(0);

}

void consumer(void *args) {

    //获取从主线程传递过来的线程编号
    int i = *((int *)args) + 1;
    for(int j = 0; j < 3; j++) {
        //当缓冲区不空时从缓冲区读取
        sem_wait(&full);
        //当缓冲区未被访问时从缓冲区读取
        pthread_mutex_lock(&mutex);

        //从缓冲区读出数据
        printf("[tid](%ld)-[consumer](%d): read from buffer success(%c)\n", (long)pthread_self(), i, buffer[out]);
        //将缓冲区读取位置移到下一位
        out = (out + 1) % 3;

        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }

    printf("[tid](%ld)-[consumer](%d): exit\n", (long)pthread_self(), i);
    pthread_exit(0);
}

int main() {

    printf("[tid](%ld): start\n", (long)pthread_self());
    pthread_mutex_init(&mutex,NULL); //初始化互斥量

    sem_init(&empty, 0, 3); //初始化信号量empty为3
    sem_init(&full, 0, 0); //初始化信号量full为0

    //主线程打开要读取的文件，获取文件描述符
    fd = open("resource.txt", O_RDONLY);

    pthread_t producers[3];
    pthread_t consumers[4];
    int args[4];

    //创建三个生产者线程

    for(int i = 0; i < 3; i++) {
        args[i] = i;
        pthread_create(&producers[i], NULL, (void*)producer, (void*)&args[i]);
    }

    //创建4个消费者线程
    for(int i = 0; i < 4; i++) {
        args[i] = i;
        pthread_create(&consumers[i], NULL, (void *)consumer, (void*)&args[i]);
    }



    //主线程等待消费者线程结束
    for(int i = 0; i < 4; i++) {
        pthread_join(consumers[i], NULL);
    }
    
    //主线程等待生产者线程结束
    for(int i = 0; i < 3; i++) {
        pthread_join(producers[i], NULL);
    }

    //主线程退出
    exit(0);
}