#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/cthread.h"

void* func0_simpleJoinedThread(void *arg) {
	printf("Eu sou a thread ID0 imprimindo %d\n", *((int *)arg));
}

void* func1_tryToJoinFunc1(void *arg) {
	printf("Eu sou a thread ID1 imprimindo %d\n", *((int *)arg));
}

void* func2_YieldExecution(void *arg) {
	printf("Eu sou a thread ID2 imprimindo antes do yield%d\n", *((int *)arg));

	cyield();

	printf("Eu sou a thread ID2 imprimindo depois do yield%d\n", *((int *)arg));
}

void* func3_SimplePrint(void *arg) {
	printf("Eu sou a thread ID3 imprimindo %d\n", *((int *)arg));
}

void* func4_SimplePrint(void *arg) {
	printf("Eu sou a thread ID4 imprimindo %d\n", *((int *)arg));
}


int main()
{
    //Pra linha do gdb warning ficar separada do resto
    putchar('\n');
    test_me();

    char* grupo = "Guilherme Cattani 243689\nAugusto Timm 113887\nGabriel Warken 179787\n";
    cidentify (grupo, strlen(grupo));

	int	id0, id1, id2, id3, id4;
	int i;

	id0 = ccreate(func0_simpleJoinedThread, (void *)&i, 0);
	id1 = ccreate(func1_tryToJoinFunc1, (void *)&i, 0);
	id2 = ccreate(func2_YieldExecution, (void *)&i, 0);
	id3 = ccreate(func3_SimplePrint, (void *)&i, 0);


	printf("Eu sou a main após a criação de ID0 e ID1\n");

	cjoin(id0);

	csuspend(id2);

	cjoin(id3);

	cresume(id2);

    cyield();

	printf("Eu sou a main voltando para terminar o programa\n");

	return 0;
}
