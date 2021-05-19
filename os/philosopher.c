#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>


pthread_mutex_t mutexs[5]; //互斥量数组
//哲学家的动作
void philosopher(void * args) {
    int i = *((int *)args);

    for(int j = 0; j < 30; j++) {
    printf("%d\n", j);
    printf("[No.%d][tid](%ld): think\n", i, (long)pthread_self());
    
    //先拿起左边的筷子
    pthread_mutex_lock(&(mutexs[i % 5]));
    printf("[No.%d][tid](%ld): get left chopstick\n", i, (long)pthread_self());

    //尝试拿起右边的筷子，如果右边的筷子被人拿了，则放下左边筷子
    int flag = pthread_mutex_trylock(&(mutexs[(i+1) % 5]));
    if(flag == 0) {
        //拿起右边的筷子
        printf("[No.%d][tid](%ld): get right chopstick\n", i, (long)pthread_self());
        //放下左边的筷子
        sleep(2);
        pthread_mutex_unlock(&(mutexs[i % 5]));
        printf("[No.%d][tid](%ld): put down left chopstick\n", i, (long)pthread_self());

        //放下右边的筷子
        pthread_mutex_unlock(&(mutexs[(i+1) % 5]));
        printf("[No.%d][tid](%ld): put down right chopstick\n", i, (long)pthread_self());

    } else {
        // 放下左边的筷子
        pthread_mutex_unlock(&(mutexs[i % 5]));
        printf("[No.%d][tid](%ld): put down left chopstick\n", i, (long)pthread_self());
    }
    
    }
    printf("[No.%d][tid](%ld): exit\n", i, (long)pthread_self());
    pthread_exit(0);

}

int main() {
  
    for(int i = 0; i < 5; i++) {
        pthread_mutex_init(&(mutexs[i]), NULL); //初始化互斥量
    } 

    
    pthread_t t[5]; //五个哲学家线程的标识
    for(int i = 0; i < 5; i++) {
        pthread_create(&(t[i]), NULL, philosopher, &i);
        pthread_join(&(t[i]),NULL);
    }

    exit(0);
}

256657