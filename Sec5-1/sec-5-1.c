/*
    Universidadde Federal do Rio de Janeiro
    EEL 770 - Sistemas Operacionais - PLE 2020
    Prof.: Rodrigo de Souza Couto

    Projeto 2 - Problema da seção 5.1: "The dining savage problem"
    Autor: Felipe Botelho Nunes da Silva

*/

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>


#define NUM_SELVAGENS        27  //Numero de selvagens para comer
#define NUM_MAX_REFEICOES    4   //Numero máximo de refeições no pote

int pote = NUM_MAX_REFEICOES;   //Armazenar o número de refeições disponívies no pote
int servidos = 0;               //Armazenar o número de selvagens que já comeram


pthread_mutex_t mutexPote = PTHREAD_MUTEX_INITIALIZER;  //Mutex para o pote

pthread_cond_t cheio = PTHREAD_COND_INITIALIZER;       //Condição de pote cheio
pthread_cond_t alerta = PTHREAD_COND_INITIALIZER;       //Condição de alerta ao cozinheiro


/*
 * Thread para o cozinheiro
*/
void *cozinheiro(void *arg) {

    while(servidos != NUM_SELVAGENS) { //Cozinheiro atua até que todos selvagens tenham comido

        pthread_mutex_lock(&mutexPote); //Adquirir o lock do pote
        while ((pote != 0) && (servidos != NUM_SELVAGENS)) { // Esperar enquanto o pote não está vazio e
                                                             // nem todos comeram ainda
            
            pthread_cond_wait(&alerta, &mutexPote);
            
        }

        //Encher o pote e avisar se ainda há selvagens para comer
        if (servidos != NUM_SELVAGENS) {
            pote = NUM_MAX_REFEICOES;
            printf("\nO cozinheiro encheu o pote!!!\n");       
            pthread_cond_broadcast(&cheio);
        }
        pthread_mutex_unlock(&mutexPote);  //Liberar o lock do pote
        
    }
    printf("\nTerminou!!!\n");
    return NULL;
}

/*
 * Thread para os selvagens
*/
void *selvagem(void *arg) {
        int id = *(int *) arg; //numero do selvagem

        pthread_mutex_lock(&mutexPote); //Adquirir o lock do pote

        while(pote == 0) { //Esperar enquanto o pote estiver vazio
            pthread_cond_wait(&cheio, &mutexPote);
        
        }
        
        pote = pote -1;           //Decrementar refeição do pote
        servidos = servidos + 1;  //Incrementar número de selvagens servidos
        printf("\n O selvagem %d comeu uma refeição do pote!!!\n", id);
        
        //pthread_cond_signal(&alerta);
        
        //Sinalizar se pote estiver vazio
        if ((pote == 0)) {
            printf("\nPOTE VAZIO!!!\n");
            pthread_cond_signal(&alerta);
        }

        
        //Se todos ja comeram, alerta o cozinheiro para terminar
        if (servidos == NUM_SELVAGENS){
            pthread_cond_signal(&alerta);
        }
        pthread_mutex_unlock(&mutexPote);
    

    return NULL;
}


/*
void *selvagem(void *arg) {

    for(int i = 0; i < NUM_SELVAGENS; i++) {

        pthread_mutex_lock(&mutexPote); //Adquirir o lock do pote

        while(pote == 0) { //Esperar enquanto o pote estiver vazio
            pthread_cond_wait(&cheio, &mutexPote);
        
        }
        
        pote = pote -1;           //Decrementar refeição do pote
        servidos = servidos + 1;  //Incrementar número de selvagens servidos
        printf("\n O selvagem %d comeu uma refeição do pote!!!\n", i+1);
        
        //pthread_cond_signal(&vazio);
        
        //Sinalizar se pote estiver vazio
        if ((pote == 0)) {
            printf("\nPOTE VAZIO!!!\n");
            pthread_cond_signal(&vazio);
        }
        pthread_mutex_unlock(&mutexPote);
    }

    return NULL;
}*/

int main() {
    int selvagem_id[NUM_SELVAGENS];
    pthread_t t_cozinheiro;
    pthread_t t_selvagens[NUM_SELVAGENS];
    //pthread_t t_selvagens;
    

    pthread_create(&t_cozinheiro, NULL, cozinheiro, NULL);
    //pthread_create(&t_selvagens, NULL, selvagem, NULL);

    //pthread_join(t_cozinheiro, NULL);
    //pthread_join(t_selvagens, NULL);
    
    
    for(int i = 0; i < NUM_SELVAGENS; i++) {
        selvagem_id[i] = i+1;
        pthread_create(&t_selvagens[i], NULL, selvagem, (void *)&selvagem_id[i]);
    }

    //pthread_join(t_cozinheiro, NULL);
    
    for (int i = 0; i < NUM_SELVAGENS; i++) {
        pthread_join(t_selvagens[i], NULL);
    }
    pthread_join(t_cozinheiro, NULL);
    

    return 0;
}
