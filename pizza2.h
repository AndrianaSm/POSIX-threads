#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>



const int nCook=2;
const int nOven=5;
const int tOrderLow=1;
const int tOrderHigh=5;
const int nOrderLow=1;
const int nOrderHigh=5;
const int tPrep=1;
const int tBake=10;
const int tHigh=15;
const int tLow=5;
const int nDeliverer=10;

void print_the_details_of_Order(int id,int tDelivered,int nPizzas,int tDistance,int freeze){
    printf("\n ID: %d \n DeliverdIn: %d \n NumberOfPizzas: %d \n Distance time: %d \n Freeze time: %d\n ====================\n",id,tDelivered,nPizzas,tDistance,freeze);
}
