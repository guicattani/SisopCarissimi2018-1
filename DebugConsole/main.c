#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/cthread.h"

void* func0(void *arg) {
	printf("Eu sou a thread ID0 imprimindo %d\n", *((int *)arg));
	return;
}

void* func1(void *arg) {
	printf("Eu sou a thread ID1 imprimindo %d\n", *((int *)arg));
}

int main()
{
    //Pra linha do gdb warning ficar separada do resto
    putchar('\n');
    test_me();

    char* grupo = "Guilherme Cattani 243689\nAugusto Timm 113887\nGabriel Warken 179787\n";
    cidentify (grupo, strlen(grupo));

	int	id0, id1;
	int i;

	id0 = ccreate(func0, (void *)&i, 0);
	id1 = ccreate(func1, (void *)&i, 0);

	printf("Eu sou a main após a criação de ID0 e ID1\n");

	cjoin(id0);
	cjoin(id1);

	printf("Eu sou a main voltando para terminar o programa\n");
}
