#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/cthread.h"

int main()
{
    //Pra linha do gdb warning ficar separada do resto
    putchar('\n');

    char* grupo = "Guilherme Cattani 243689\nAugusto Timm 113887\nGabriel Warken 179787\n";

    int returno = cidentify (grupo, strlen(grupo));


    test_me();
    return 0;
}
