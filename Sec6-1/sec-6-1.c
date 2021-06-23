/*
    Universidadde Federal do Rio de Janeiro
    EEL 770 - Sistemas Operacionais - PLE 2020
    Prof.: Rodrigo de Souza Couto

    Projeto 2 - Problema da seção 6.1: "The search-insert-delete problem"
    Autor: Felipe Botelho Nunes da Silva

*/

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define NUM_INSERTER_THREADS    3  //Numero de threads de inserters
#define NUM_SEARCHER_THREADS    4  //Numero de threads de searchers
#define NUM_DELETER_TREADS      5  //Numero de threads de deleters

pthread_mutex_t mutexInserter = PTHREAD_MUTEX_INITIALIZER;  //mutex para inserter
pthread_mutex_t mutexSearcher = PTHREAD_MUTEX_INITIALIZER;  //mutex para searcher
pthread_mutex_t mutexDeleter = PTHREAD_MUTEX_INITIALIZER;   //mutex para deleter

pthread_cond_t okInserter = PTHREAD_COND_INITIALIZER;  //Condição que sinaliza estado ok para inserter atuar
pthread_cond_t okSearcher = PTHREAD_COND_INITIALIZER;  //Condição que sinaliza estado ok para searcher atuar
pthread_cond_t okDeleter = PTHREAD_COND_INITIALIZER;   //Condição que sinaliza estado ok para deleter atuar

int searcher_ativos = 0;  //Numero de threads de searcher ativas
int inserter_ativo = 0;   //Indica com 1 se ha thread de inserter na lista, 0 se não
int deleter_ativo = 0;   //Indica com 1 se ha thread de deleter na lista, 0 se não

/*
* Thread para searchers
*/
void *searcher(void *arg) {
    int id = *(int *) arg; //Identificador da thread searcher

    pthread_mutex_lock(&mutexSearcher); //Adquirir lock para alterar numero de searchers

    //Verificar se há condição de seacher atuar, esperar se não tiver
    while(deleter_ativo > 0) {
        pthread_cond_wait(&okSearcher, &mutexSearcher);
    }
    searcher_ativos++;
    pthread_mutex_unlock(&mutexSearcher);  //Liberar lock, pois é permitido mais de 1 searcher na lista

    printf("O searcher %d está procurando!!!\n", id); 

    sleep(1);  //Esperar por um tempo só para simular uma busca

    pthread_mutex_lock(&mutexSearcher);        //Adquirir lock para alterar numero de searchers
    printf("O searcher %d terminou!!!\n", id); 
    searcher_ativos--;

    //Sinalizar se tiver condição de thread deleter atuar
    if ((searcher_ativos == 0) && (inserter_ativo == 0)) {
        pthread_cond_signal(&okDeleter);
    }
    pthread_mutex_unlock(&mutexSearcher);
   
    return NULL;
}

/*
* Thread para inserters
*/
void *inserter(void *arg) {
    int id = *(int *) arg;  //Identificador da thread inserter

    pthread_mutex_lock(&mutexInserter);  //Adquirir lock para alterar inserter ativo

    //Verificar se há condição do inserter atuar, esperar se não tiver
    while ((inserter_ativo > 0) || (deleter_ativo > 0)) {
        pthread_cond_wait(&okInserter, &mutexInserter);
    }
    inserter_ativo = 1;

    printf("O inserter %d está inserindo!!!\n", id);  

    sleep(2); // Esperar um tempo só para simular inserção

    printf("O inserter %d terminou!!!\n", id);  //Indicar finalização da thread
    inserter_ativo = 0;

    pthread_cond_signal(&okInserter); //Sinalizar que outro inserter pode atuar

    //Sinalizar caso uma thread deleter possa atuar
    if (searcher_ativos == 0) {
        pthread_cond_signal(&okDeleter);
    }
    pthread_mutex_unlock(&mutexInserter);

    return NULL;
}

/*
* Thread para deleter
*/
void *deleter(void *arg) {
    int id = *(int *) arg;  //Identificador de thread deleter

    pthread_mutex_lock(&mutexDeleter);  //Adquirir lock para alterar deleter ativo

    //Verificar se há condição de deleter atuar, esperar se não tiver
    while ((searcher_ativos > 0) || (inserter_ativo > 0) || (deleter_ativo > 0)) {
        pthread_cond_wait(&okDeleter, &mutexDeleter);
    }   

    deleter_ativo = 1;
    printf("O deleter %d está deletando!!!\n", id);

    sleep(3); //Esperar um tempo só para simular um delete

    printf("O deleter %d terminou!!!\n", id);
    deleter_ativo = 0;

    //Sinalizar para outras threads que finalizou, qualquer outra pode atuar
    pthread_cond_broadcast(&okSearcher);
    pthread_cond_signal(&okInserter);
    pthread_cond_signal(&okDeleter);
    pthread_mutex_unlock(&mutexDeleter);

    return NULL;
}

int main() {
    int searcher_id[NUM_SEARCHER_THREADS];   //Vetor de ids para threads searchers
    int inserter_id[NUM_INSERTER_THREADS];   //Vetor de ids para threads inserters
    int deleter_id[NUM_DELETER_TREADS];      //Vetor de ids para threads deleters

    pthread_t t_searcher[NUM_SEARCHER_THREADS];   //Vetor de threads searchers
    pthread_t t_inserter[NUM_SEARCHER_THREADS];   //Vetor de threads inserters
    pthread_t t_deleter[NUM_DELETER_TREADS];      //Vetor de threads deleters


    //Criação das threads, associando aos respectivos ids
    for(int i = 0; i < NUM_SEARCHER_THREADS; i++) {
        searcher_id[i] = i+1;
        pthread_create(&t_searcher[i], NULL, searcher, (void *)&searcher_id[i]);
    }

    for(int i = 0; i < NUM_INSERTER_THREADS; i++) {
        inserter_id[i] = i+1;
        pthread_create(&t_inserter[i], NULL, inserter, (void *)&inserter_id[i]);
    }

    for(int i = 0; i < NUM_DELETER_TREADS; i++) {
        deleter_id[i] = i+1;
        pthread_create(&t_deleter[i], NULL, deleter, (void *)&deleter_id[i]);
    }

    for(int i = 0; i < NUM_SEARCHER_THREADS; i++) {
        pthread_join(t_searcher[i], NULL);
    }

    for(int i = 0; i < NUM_INSERTER_THREADS; i++) {
        pthread_join(t_inserter[i], NULL);
    }

    for(int i = 0; i < NUM_DELETER_TREADS; i++) {
        pthread_join(t_deleter[i], NULL);
    }

    return 0;
}