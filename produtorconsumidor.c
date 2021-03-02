#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define PACOTES 3
#define SLEEP_TIME 5

typedef struct pacote {
    int numPacote;
    pthread_t idProdutor;
}pkg;

pkg *buffer;
int in = 0;
int out = 0;

sem_t cheio;
sem_t vazio;
pthread_mutex_t mutex;

unsigned int t;

void *produtor(void *p) {
    pkg pacote;

    for(int i = 0; i < PACOTES; i++) {
        sleep(rand_r(&t) % SLEEP_TIME + 1);
        sem_wait(&vazio);
        pthread_mutex_lock(&mutex);

        pacote.numPacote = rand()%10;
        pacote.idProdutor = pthread_self();

        buffer[in] = pacote;
        printf("Produtor %d inserindo pacote %d no buffer\n", (unsigned int)pthread_self()%10, buffer[in].numPacote);
        in++;
        in = (in+1)%10;

        pthread_mutex_unlock(&mutex);
        sem_post(&cheio);

    }

}

void *consumidor(void *c) {

    for(int i = 0; i < PACOTES; i++) {
        sleep(rand_r(&t) % SLEEP_TIME + 1);
        sem_wait(&cheio);
        pthread_mutex_lock(&mutex);
        pkg pacote = buffer[out];
        printf("Consumidor %d removendo pacote %d do produtor %d\n", (unsigned int)pthread_self()%10, pacote.numPacote, (unsigned int)pacote.idProdutor%10);
        out++;
        out = (out+1)%10;
        pthread_mutex_unlock(&mutex);
        sem_post(&vazio);
    }

}


int main(int argc, char*argv[]) {

    int produtorQtd;
    int consumidorQtd;
    int bufferSize;

    printf("Quantidade de produtores: ");
    scanf("%d", &produtorQtd);

    printf("Quantidade de consumidores: ");
    scanf("%d", &consumidorQtd);

    printf("Tamanho do buffer: ");
    scanf("%d", &bufferSize);

    buffer = (pkg *)malloc(sizeof(pkg)*bufferSize);

    printf("\nIniciando produtores...\n");
    printf("Iniciando consumidores...\n\n");

    int prods[produtorQtd];
    int consums[consumidorQtd];

    t = (unsigned int)time(NULL);
    pthread_mutex_init(&mutex, NULL);
    sem_init(&vazio, 0, bufferSize);
    sem_init(&cheio, 0, 0);

    pthread_t pro[produtorQtd];
    for(int i = 0; i < produtorQtd; i++) {
        pthread_create(&pro[i], NULL, (void *)produtor, (void *)&prods[i]);
    }

    pthread_t con[consumidorQtd];
    for (int i = 0; i < consumidorQtd; i++) {
        pthread_create(&con[i], NULL, (void *)consumidor, (void *)&consums[i]);
    }

    sleep(SLEEP_TIME);

    for(int i = 0; i < produtorQtd; i++){
        pthread_join(pro[i], NULL);
    }

    printf("\nProdutores terminaram.\n\n");

    for(int i = 0; i < consumidorQtd; i++){
        pthread_join(con[i], NULL);
    }

    printf("\nConsumidores terminaram.\n\n");

    pthread_mutex_destroy(&mutex);
    sem_destroy(&vazio);
    sem_destroy(&cheio);

    printf("\nFinalizando programa.\n");

    return 0;

}