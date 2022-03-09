#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <time.h>
void Error(char* msg){fprintf(stderr,"Error:%s\n",msg); exit(1);}
#define M 10
int now = 0;

//sems
sem_t out;// patient out
sem_t in; // patient in
sem_t sofa;// patient on sofa
sem_t treat;// patient getting treatment
sem_t payment;// patient pay
sem_t paid;// done 
sem_t mutex;// mutex
sem_t work;// doctor working
sem_t getMoney;// doctor get paid

//node for prio..
typedef struct ourNode{
int val;
struct ourNode* next;
}*Onode;
Onode onSofaNow = NULL;// list for siting patient
Onode notOnSofaNow = NULL;// list for in clinic but no siting
Onode pNew(Onode head , int num);
Onode Go(Onode head,int* curStand);
// functions for patients
/******/
void enter(int i){// func for patient to enter clinic useing sems
sem_wait(&mutex);
notOnSofaNow = pNew(notOnSofaNow,i);//
sem_post(&mutex);
sem_wait(&mutex);
now++;
sem_post(&mutex);
printf("I'm Patient #%d, I got into the clinic\n",i);
sleep(1);
sem_wait(&sofa);
}
/******/
void sit(int i){
sem_wait(&mutex);
notOnSofaNow = Go(notOnSofaNow,&i);//
sem_post(&mutex);
printf("I'm Patient #%d, I'm setting on the sofa\n",i);
onSofaNow = pNew(onSofaNow,i);//
sleep(1);
sem_wait(&treat);
}
/******/
void getMid(int i){
sem_wait(&mutex);
onSofaNow = Go(onSofaNow,&i);//
sem_post(&mutex);
printf("I'm Patient #%d, I'm getting treatment\n",i);
sleep(1);
sem_post(&sofa);
sem_post(&work);
}
/******/
void pay(int i){
sem_wait(&payment);
printf("I'm Patient #%d, I'm paying now\n",i);
sem_post(&getMoney);
sem_wait(&paid);
sleep(1);
}
void notIn(int i){
sem_post(&mutex);
printf("I'm Patient #%d, I'm out of clinic now\n",i);
sleep(1);
sem_wait(&out);
}

// functions for doctor
/******/
void mid(int i){
sem_wait(&work);
printf("I'm Dental Hygienist #%d, I'm working now\n",i);
sem_post(&treat);
}
/******/
void getPayed(int i){
sem_post(&payment);
sleep(1);
sem_wait(&getMoney);
printf("I'm Dental Hygienist #%d, I'm getting a payment\n",i);
sem_post(&paid);
sleep(1);
sem_wait(&mutex);
now--;
sem_post(&mutex);
sem_post(&out);
}
/******/
void* patient(void* arg) {
int i = *((int*)arg);
while (1){
sem_wait(&mutex);
if(now < M){
sem_post(&mutex);
enter(i); 
sit(i);
getMid(i);
pay(i);}
if(now>= M)
notIn(i);
}}

/******/
void* doctor(void* arg) {
int i = *((int*)arg);
while (1){
mid(i); 
getPayed(i); }}

///******/main
int main(int argc,char* argv[]){
// sems
sem_init(&out,0,0);
sem_init(&in,0,1);
sem_init(&sofa,0,4);
sem_init(&treat,0,3);
sem_init(&mutex,0,1);
sem_init(&payment,0,1);
sem_init(&paid,0,1);
sem_init(&work,0,0);
sem_init(&getMoney,0,0);
// threads
int i,docIdx[3],patIdx[M+2];
pthread_t doctors[3],patients[M+2]; 
//threads for doctors        
for(i=0;i<3;i++){
docIdx[i]=i+1;
if(pthread_create(&doctors[i], NULL, doctor,(void*)&docIdx[i])!=0)
Error("thread fail"); 
}
//threads for patients 
for(i=0;i<M+2;i++){
patIdx[i]=i+1;
if(pthread_create(&patients[i], NULL, patient,(void*)&patIdx[i])!=0)
Error("thread fail"); 
}
for (i = 0; i < 3; i++) {
if(pthread_join(doctors[i], NULL) != 0)
Error("joining error!!");}
for (i = 0; i < M+2; i++) {
if(pthread_join(patients[i], NULL) != 0)
Error("joining error!!");}
return 0;
}

Onode pNew(Onode head , int num){ 
Onode temp = (Onode)malloc(sizeof(struct ourNode)); // new node
if(!temp) exit(1);// if error exit
temp->val = num; // new node val
temp->next = NULL; 
if(head == NULL){ // if list is empty
head = temp; } 
else{ // if not empty
Onode temp2; 
for(temp2 = head ; temp2->next ;temp2=temp2->next); 
temp2->next = temp; } 
return head; 
} 

Onode Go(Onode head,int* val) { 
if(head == NULL) return head; // if empty
Onode temp = head; // new 
*val = head->val; // currnet val
head = head->next; 
free(temp); // free 
return head; 
}





