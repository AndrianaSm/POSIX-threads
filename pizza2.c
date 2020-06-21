#include "pizza2.h"

//Variables
unsigned int seed;
int available_Cook;
int available_Oven;
int available_Deliver;
int max_time;
int total_time;
int total_freeze_time;
int max_freeze_time;

//Conditions
pthread_cond_t cond_available_Cook = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_available_Oven = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_available_Deliverer = PTHREAD_COND_INITIALIZER;

//MUTEX
pthread_mutex_t mutex_available_Cook = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_available_Oven = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_available_Deliverer = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mutext_max_time = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutext_max_freeze_time = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutext_total_time = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutext_total_freeze_time = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mutext_print = PTHREAD_MUTEX_INITIALIZER;


void *procedure(void * idCust) {
    struct timespec orderStart;
    struct timespec orderFinish;
    struct timespec orederBaked;
    int custumerId =  *(int *)idCust;
    int nPizza=rand_r(&seed)%nOrderHigh+nOrderLow;
    int tDistance=rand_r(&seed)%(tHigh-tLow+1)+tLow;

    //get the start time
    clock_gettime(CLOCK_REALTIME, &orderStart);

    //lock the availible Cook mutex
    pthread_mutex_lock(&mutex_available_Cook);

    //chek if there are available cookers
    while(available_Cook<=0) {
        pthread_cond_wait(&cond_available_Cook,&mutex_available_Cook);
    }

    //decreasing the available cookers
    --available_Cook;

    //unclock theavailible Cook mutex
    pthread_mutex_unlock(&mutex_available_Cook);

    //Prepare the pizza fornPizza*tPrep sec
    sleep(nPizza*tPrep);


    //lock the availible Oven mutex
    pthread_mutex_lock(&mutex_available_Oven);

    //chek if there are available Ovens
    while(available_Oven<=0) {
        pthread_cond_wait(&cond_available_Oven,&mutex_available_Oven);
    }

    //decreasing the available Ovens
    --available_Oven;

    //unclock the availible Oven mutex
    pthread_mutex_unlock(&mutex_available_Oven);

    //increasing the available Cookers
    pthread_mutex_lock(&mutex_available_Cook);
    ++available_Cook;
    //signal for available Cooker condition
    pthread_cond_signal(&cond_available_Cook);
    pthread_mutex_unlock(&mutex_available_Cook);

    //bake the pizza for tBake seconds
    sleep(tBake);
    clock_gettime(CLOCK_REALTIME, &orederBaked);

    //lock the availible Deliverer mutex
    pthread_mutex_lock(&mutex_available_Deliverer);
    //chek if there are available Deliverer
    while(available_Deliver<=0) {
        pthread_cond_wait(&cond_available_Deliverer,&mutex_available_Deliverer);
    }
    //decreasing the available Deliverer
    --available_Deliver;
    //unclock theavailible Deliverer mutex
    pthread_mutex_unlock(&mutex_available_Deliverer);

    //increasing the available ovens
    pthread_mutex_lock(&mutex_available_Oven);
    ++available_Oven;
    //signal for available Oven condition
    pthread_cond_signal(&cond_available_Oven);
    pthread_mutex_unlock(&mutex_available_Oven);

    //Sleep for tDistance
    sleep(tDistance);

    //#####THE ORDER IS DELIVERED######
    clock_gettime(CLOCK_REALTIME, &orderFinish);
    int total_sec =orderFinish.tv_sec-orderStart.tv_sec;
    int freeze_sec =orderFinish.tv_sec-orederBaked.tv_sec;

    //Print the order
    pthread_mutex_lock(&mutext_print);
    print_the_details_of_Order(custumerId,total_sec,nPizza,tDistance,freeze_sec);
    pthread_mutex_unlock(&mutext_print);

    //update the total time
    pthread_mutex_lock(&mutext_total_time);
    total_time+=total_sec;
    pthread_mutex_unlock(&mutext_total_time);

    //update the total freeze time
    pthread_mutex_lock(&mutext_total_freeze_time);
    total_freeze_time+=freeze_sec;
    pthread_mutex_unlock(&mutext_total_freeze_time);

    //update the max waiting time
    pthread_mutex_lock(&mutext_max_time);
    if(max_time<total_sec){
        max_time=total_sec;
    }
    pthread_mutex_unlock(&mutext_max_time);

    //update the max freeze time
    pthread_mutex_lock(&mutext_max_freeze_time);
    if(max_freeze_time<freeze_sec){
        max_freeze_time=freeze_sec;
    }
    pthread_mutex_unlock(&mutext_max_freeze_time);

    //Wait the deliverer to return
    sleep(tDistance);

    //increasing the available Deliverer
    pthread_mutex_lock(&mutex_available_Deliverer);
    ++available_Deliver;
    //signal for available Deliverer condition
    pthread_cond_signal(&cond_available_Deliverer);
    pthread_mutex_unlock(&mutex_available_Deliverer);

    pthread_exit(NULL);

}

int main (int argc, char *argv[]) {

    //invalid number of arguments
    if(argc !=3) {
        printf("Invalid number of arguments!\nPlease declare the number of clients and the seed \n" );
        exit(-1);
    }

    //initialize the number of clients
    int nCust = atoi(argv[1]);

    //number of clients musy be positive
    if(nCust<1) {
        printf("The number of clients must be positive\n" );
        exit(-1);
    }
    available_Cook = nCook;
    available_Oven = nOven;
    available_Deliver = nDeliverer;
    total_time=0;
    total_freeze_time=0;
    max_time=0;
    max_freeze_time=0;
    //initialize sed
    seed = atoi(argv[2]);

    printf("CLINETS = %d \nSEED = %d\n",nCust,seed );

    //Initialize custumers id
    int idCusts[nCust];
    for(int i=0 ; i<nCust ; i++) {
        idCusts[i] = i;
    }

    //Create the threads
    pthread_t threads[nCust];

    for(int i=0; i<nCust; ++i) {
        int random_next_Order = rand_r(&seed)%tOrderHigh+tOrderLow;;
        pthread_create(&threads[i], NULL, &procedure, &idCusts[i]);
        random_next_Order = rand_r(&seed)%tOrderHigh+tOrderLow;
        sleep(random_next_Order);
    }

    //Wait all threads to finish
    for(int i=0; i<nCust; ++i) {
        pthread_join(threads[i], NULL);
    }

    div_t output;

    output =div(total_time,nCust);
    printf("\nAVERAGE WAITING TIME : %d" ,output.quot);
    printf("\nMAX WAITING TIME : %d",max_time);

    output =div(total_freeze_time,nCust);
    printf("\nAVERAGE FREEZE TIME : %d" ,output.quot);
    printf("\nMAX FREEZE TIME = %d\n",max_freeze_time);


    return 0;
}
