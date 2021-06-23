/*
    Universidadde Federal do Rio de Janeiro
    EEL 770 - Sistemas Operacionais - PLE 2020
    Prof.: Rodrigo de Souza Couto

    Projeto 2 - Problema da seção 7.1: "The sushi bar problem"
    Autor: Felipe Botelho Nunes da Silva

*/

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define NUM_TOTAL_PESSOAS  12   //Numero de threads de pessoas
#define MAX_SENTADOS        5   //Numero maximo de pessoas sentadas


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; //mutex para proteger as variaveis que serão alteradas

pthread_cond_t vazio = PTHREAD_COND_INITIALIZER;  //Condição que sinaliza quando os assentos estão vagos

int esperando = 0;    //Indica o numero de pessoas esperando
int comendo = 0;      //Indica o numero de pessoas comendo
int deveEsperar = 0;  //1 se deve esperar, 0 se não


/*
*  Função que retorna o valor mínimo entre dois números
*/
int minimo(int a, int b) {
    if (a > b) {
        return b;
    }
    else {
        return a;
    }
}


/*
* Thread para as pessoas
*/
void *pessoas(void *arg) {
    int id = *(int *) arg;  //Identificador da thread
    int numMin = 0;         //Variavel auxiliar

    pthread_mutex_lock(&mutex);  //Adquirir o lock

    //Verificar se a pessoa deve esperar
    while (deveEsperar != 0) {
        esperando++;
        printf("A pessoa %d está esperando!!!\n", id);
        pthread_cond_wait(&vazio, &mutex);
        esperando--;
    }

    comendo++;
    if (comendo == 5) { //Se for a quinta pessoa, atualizar a variavel deveEsperar
        deveEsperar = 1;
    }
    pthread_mutex_unlock(&mutex);  //Liberar o lock, pois é permitido que até cinco pessoas estejam comendo

    //Comer
    printf("A pessoa %d está comendo!!!\n", id);
    sleep(2);  //Esperar um tempo para simular a refeição 

    //Saindo do bar
    pthread_mutex_lock(&mutex);  //Adquirir o lock para alterar variavel
    comendo--;
    printf("A pessoa %d terminou de comer!!!\n", id);

    //Se for a última a sair, analisar quantas pessoas esperando (no máximo 5) devem acordar
    if(comendo == 0) {
        numMin = minimo(5, esperando);
        deveEsperar = 0;
        for(int i = 0; i < numMin; i++) {
            pthread_cond_signal(&vazio);
        }
    }
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main() {
    int pessoas_id[NUM_TOTAL_PESSOAS];       //Vetor de ids para threads pessoas
    pthread_t t_pessoas[NUM_TOTAL_PESSOAS];  //Vetor de threads de pessoas

    //Criação das threads, associando aos respectivos ids
    for(int i = 0; i < NUM_TOTAL_PESSOAS; i++) {
        pessoas_id[i] = i+1;
        pthread_create(&t_pessoas[i], NULL, pessoas, (void *)&pessoas_id[i]);
    }

    for(int i = 0; i < NUM_TOTAL_PESSOAS; i++) {
        pthread_join(t_pessoas[i], NULL);
    }

    return 0;
}