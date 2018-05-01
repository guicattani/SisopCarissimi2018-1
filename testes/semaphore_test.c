#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/cthread.h"
#include "../include/cdata.h"

#define SIZEIDENTIFY 67

csem_t semaphore;

/**
TESTES DE SEMAFORO
**/

void* func0_GetResource(void *arg) {
	printf("SEMAFORO Eu sou a thread ID1 PEGANDO o recurso %d\n", *((int *)arg));
	cwait(&semaphore);
	cyield();
	csignal(&semaphore);
    printf("SEMAFORO Eu sou a thread ID1 LIBERANDO o recurso %d\n", *((int *)arg));
	return NULL;
}

void* func1_TryGetResource(void *arg) {
	printf("SEMAFORO Eu sou a thread ID2 PEGANDO o recurso %d\n", *((int *)arg));
	cwait(&semaphore);
	csignal(&semaphore);
    printf("SEMAFORO Eu sou a thread ID2 LIBERANDO o recurso %d\n", *((int *)arg));
	return NULL;
}

void* func2_TryAgainToGetResource(void *arg) {
	printf("SEMAFORO Eu sou a thread ID3 PEGANDO o recurso %d\n", *((int *)arg));
	cwait(&semaphore);
	csignal(&semaphore);
    printf("SEMAFORO Eu sou a thread ID3 LIBERANDO o recurso %d\n", *((int *)arg));
	return NULL;
}
void* func3_TryEvenBetterToGetResource(void *arg) {
	printf("SEMAFORO Eu sou a thread ID4 PEGANDO o recurso %d\n", *((int *)arg));
	cwait(&semaphore);
	csignal(&semaphore);
    printf("SEMAFORO Eu sou a thread ID4 LIBERANDO o recurso %d\n", *((int *)arg));
	return NULL;
}

int main()
{
    char groupName[SIZEIDENTIFY+1];
    cidentify (groupName, SIZEIDENTIFY);

    printf ("Nome do grupo:\n%s\n", groupName);

    printf("\nAgora vamos testar os semáforos...\n");

    /**
    TESTES DE SEMAFORO
    **/

    csem_init(&semaphore, 1);

    int i = 173;
    ccreate(func0_GetResource, (void *)&i, 0);
	ccreate(func1_TryGetResource, (void *)&i, 0);
	ccreate(func2_TryAgainToGetResource, (void *)&i, 0);
	ccreate(func3_TryEvenBetterToGetResource, (void *)&i, 0);

	cyield();
	cyield();
	cyield();
	cyield();
	cyield();

    printf("\nFinalmente terminamos!\n");
	return SUCCESS;
}
