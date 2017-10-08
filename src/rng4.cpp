/********************************************************************
*
*
* File: rcg4.cpp
* Description: Modify the writer function so it is launched in a different thread. 
*   The random number generator should put the numbers and timestamps into a queue that the writer function uses to write its output to disk.
*
*
* Version History:
*	08 Oct 2017		1.00		JH		First Revision
*   08 Oct 2017		1.10		JH		Change to method
*   08 Oct 2017		1.20		JH		Add writer function
*   08 Oct 2017		1.30		JH		Launch in separate thread
*
********************************************************************/

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

/*
**	class NumberAndTime
**		summary:
**			class to get current number and time
**			
**		
**
*/
class NumberAndTime {
public:
    int number;
    time_t time;
    pthread_t tid;

    NumberAndTime(int n, time_t t, pthread_t tid) {
        this->number = n;
        this->time = t;
        this->tid = tid;
    }
    ~NumberAndTime() {
    }
};


/*
**	class ThreadSafeBlockingQueue
**		summary:
**			threadsafe queue with mutex to avoid issues with shared resources
**			
**		
**
*/
class ThreadSafeBlockingQueue {
private:
    int size; 
    NumberAndTime **nts;
    int first;
    int last;
    pthread_mutex_t mutex;
    pthread_cond_t empty;
    pthread_cond_t full;

public:
    ThreadSafeBlockingQueue(int size) {
        this->size = size;
        nts = new NumberAndTime*[size];
        first = -1;
        last = -1;
        pthread_mutex_init(&mutex, NULL);
        pthread_cond_init (&empty, NULL);
        pthread_cond_init (&full, NULL);
    }
    ~ThreadSafeBlockingQueue() {
        delete nts;
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&empty);
        pthread_cond_destroy(&full);
    }
    
    void enq(NumberAndTime *nt) {
        pthread_mutex_lock(&mutex);
        while (last != -1 && ((last + 1) % size) == first) {
            // while queue is full, wait on full condition
            pthread_cond_wait(&full, &mutex);
        }
        last = (last + 1) % size;
        nts[last] = nt;
        if (first == -1) {
            // if queue was empty, now it has one item
            first = last;
        }
        //printf("enq %d, %d, %d\n", nt->number, nt->time, pthread_self());
        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&mutex);
    }
    
    NumberAndTime *deq() {
        NumberAndTime *nt;
        timespec to;
        pthread_mutex_lock(&mutex);
        // return NULL if queue is empty for 3 seconds 
        to.tv_sec = time(NULL) + 3;
        to.tv_nsec = 0;
        while (first == -1) {
            // while queue is empty, wait on empty condition
            int err = pthread_cond_timedwait(&empty, &mutex, &to);
            if (err == ETIMEDOUT) {
                //printf("deq TIMEOUT\n");
                pthread_cond_signal(&full);
                pthread_mutex_unlock(&mutex);
                return NULL;
            }
        }
        nt = nts[first];
        if (first == last) {
            // if queue had only one item, now make it empty
            first = -1;
            last = -1;
        } else {
            first = (first + 1) % size;
        }
        //printf("deq %d, %d, %d\n", nt->number, nt->time, nt->tid);
        pthread_cond_signal(&full);
        pthread_mutex_unlock(&mutex);
        return nt;
    }
    
};

/*
**	class RNG
**		summary:
**			simple RNG class that will generate weighted 
**          value between 1-5 
**			
**		
**
*/
class RNG {
private:
    ThreadSafeBlockingQueue *queue;
    int count[5] = {0, 0, 0, 0, 0}; 
    void enq(int r);

public:
    RNG(ThreadSafeBlockingQueue *queue);
    ~RNG();
    int generate();
    double getFrequency(int r);
};

RNG::RNG(ThreadSafeBlockingQueue *queue) {
    this->queue = queue;
}

RNG::~RNG() {
}

/*
**	generate()
**		summary:
**			Generates weighted random value between 1-5 
**			
**		parameters:
**			none
**		returns:
**          int value between 1-5			
**
*/
int RNG::generate() {
    int r = rand() % 100;
    if (r < 50) {
        enq(1);
        return 1;
    }
    if (r < 75) {
        enq(2);
        return 2;
    }
    if (r < 90) {
        enq(3);
        return 3;
    }
    if (r < 95) {
        enq(4);
        return 4;
    }
    enq(5);
    return 5;
}

/*
**	getFrequency()
**		summary:
**			Generates weighted random value between 1-5 
**			
**		parameters:
**			int value between 1-5
**		returns:
**          int count of value 			
**
*/
double RNG::getFrequency(int r) {
    if (r < 1 || r > 5) {
        return 0;
    }
    int total = 0;
    for (int i = 0; i < 5; i++) {
        total = total + count[i];
    }
    return count[r-1] * 100 / total;
}

/*
**	enq()
**		summary:
**			Enqueues last generated value with max size 100
**			
**		parameters:
**			int value between 1-5
**		returns:
**          none		
**
*/
void RNG::enq(int r) {
    count[r-1]++;
    time_t now;
    time ( &now );
    NumberAndTime *nt = new NumberAndTime(r, now, pthread_self());
    queue->enq(nt);
}

/*
**	task()
**		summary:
**			Processes task 
**			
**		parameters:
**			*queue
**		returns:
**          none		
**
*/
void *task(void *queue) {
    printf("START DEQ THREAD\n");
    FILE *f = fopen("rcg.out", "w");
    
    int i = 0;
    while (true) {
        NumberAndTime *nt = ((ThreadSafeBlockingQueue *)queue)->deq();
        if (nt == NULL) {
            break;
        }
        printf("%d: Generated %d on %d from thread: %d\n", i, nt->number, nt->time, nt->tid);
        fprintf(f, "%d: Generated %d on %d from thread: %d\n", i, nt->number, nt->time, nt->tid);
        i++;
    }
    fclose(f);
    printf("END DEQ THREAD\n");
}


/* 
** main() 
**      Driver function to test 
**
*/ 
int main()
{
    int qsize = 30;
    ThreadSafeBlockingQueue *queue = new ThreadSafeBlockingQueue(qsize);
    
    pthread_t deqThread;
    pthread_create( &deqThread, NULL, task, (void*) queue);
    
    RNG *rng = new RNG((ThreadSafeBlockingQueue *)queue);
    
    for (int i = 0; i < 100; i++) {
        int r = rng->generate();
        //usleep(10000);
    }
    delete rng;
    
    pthread_join(deqThread, NULL);
   
    delete queue;
    printf("END\n");
}
