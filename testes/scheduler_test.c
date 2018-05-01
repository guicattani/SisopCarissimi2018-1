#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/cthread.h"
#include "../include/cdata.h"

#define SIZEIDENTIFY 67

void* func1_simpleJoinedThread(void *arg) {
	printf("Eu sou a thread ID1 imprimindo %d\n", *((int *)arg));
	cyield();

	return NULL;
}

void* func2_tryToJoinFunc1(void *arg) {
    int status = cjoin(*((int *)arg));
	printf("Eu sou a thread ID2 imprimindo %d\n", *((int *)arg));
	if(status < 0)
        printf("Recebi um erro por tentar dar join em uma thread já esperada por outra: %d\n", *((int *)arg));

    return NULL;
}

void* func3_YieldExecution(void *arg) {
	printf("Eu sou a thread ID3 imprimindo antes do yield %d\n", *((int *)arg));
	cyield();
	printf("Eu sou a thread ID3 imprimindo depois do yield %d\n", *((int *)arg));

	return NULL;
}

void* func4_SimplePrint(void *arg) {
	printf("Eu sou a thread ID4 imprimindo %d\n", *((int *)arg));
	return NULL;
}

int main()
{
    char groupName[SIZEIDENTIFY+1];
    cidentify (groupName, SIZEIDENTIFY);

    printf ("Nome do grupo:\n%s\n", groupName);

    /**
    TESTES DE JOIN, YIELD SUSPEND E RESUME
    **/

	int	id1, id2, id3, id4;
	int i = 173;

	id1 = ccreate(func1_simpleJoinedThread, (void *)&i, 0);
	id2 = ccreate(func2_tryToJoinFunc1, (void *)&id1, 0);
	id3 = ccreate(func3_YieldExecution, (void *)&i, 0);
	id4 = ccreate(func4_SimplePrint, (void *)&i, 0);


	printf("Eu sou a main após a criação de ID1, ID2, ID3, ID4 e ID5\n");

	cjoin(id1);
	cjoin(id2);
	csuspend(id3);
	cjoin(id4);
	cresume(id3);
    cyield();

	printf("\nEu sou a main voltando para terminar o programa\n");

    printf("\nFinalmente terminamos!\n");
	return SUCCESS;
}
