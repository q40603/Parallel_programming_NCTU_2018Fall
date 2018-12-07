#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
unsigned long long count_in_circle;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
//static unsigned int seed = 35791246;
void* toss(void *arg) {
    // var
    unsigned long long count = 0;
    unsigned int seed = 35791246;
    unsigned long long round_per_thread = (unsigned long long) arg;
    // init
    for (unsigned long long i = 0; i < round_per_thread; i++) {
        // var
        double x = (double) rand_r(&seed) / RAND_MAX ;
        double y = (double) rand_r(&seed) / RAND_MAX;
        if(x * x + y * y <= 1) count++;
    }
    // critical section
    pthread_mutex_lock(&lock);
    count_in_circle += count;
    pthread_mutex_unlock(&lock);
    return NULL;
}

int main(int argc, char *argv[]) {

    // var declare
    double pi_estimate;
    unsigned long long num_of_cpu = atoll(argv[1]) ;
    unsigned long long num_of_toss = atoll(argv[2]);
    count_in_circle = 0;
    unsigned long long round_per_thread = num_of_toss / num_of_cpu;
    unsigned long long round_main_thread = round_per_thread + num_of_toss % num_of_cpu ;
    pthread_attr_t attr;
    pthread_t* threads = malloc(num_of_cpu * sizeof(pthread_t));
    pthread_attr_init(&attr);
    pthread_mutex_init(&lock, NULL);


    for (unsigned long long i = 0; i < num_of_cpu; i++) {
        if (i == 0) pthread_create(&threads[i], &attr, toss, (void *) round_main_thread);
        else{
            pthread_create(&threads[i], &attr, toss, (void *) round_per_thread);
        }
    }
    for (unsigned long long i = 0; i < num_of_cpu; i++) {
        pthread_join(threads[i], NULL);
    }

    // end measurement
    pi_estimate = (double)count_in_circle * 4 / num_of_toss;
    printf("%lf\n", pi_estimate);
    return 0;
}