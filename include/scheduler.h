#include "../include/support.h"
#include "../include/cdata.h"
#include <ucontext.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define SUCCESS 0;
#define ERROR -1;

/**
getContextToFinishProcess
    devolve um contexto que vai estar atrelado a uma função que termina a execução da thread
Parâmetros:
    void
Retorno:
    contexto da função de terminar thread
**/
ucontext_t* getContextToFinishProcess();


/**
getExecutingThread
    devolve a thread em execução
Parâmetros:
    void
Retorno:
    contexto da função de terminar thread
**/
TCB_t* getExecutingThread();


/**
finishThread
    termina a thread e libera a memória que ela ocupava
Parâmetros:
    TCB_t* thread, a thread a ser liberada da memória
Retorno:
    Se correto:         devolve 0
    Se erro ocorreu:    devolve < 0
**/
void finishThread();


/**
schedulerInitialize
    Inicializa todas filas
Parâmetros:
    void
Retorno:
    void
**/
void schedulerInitialize(void);


/**
getNewTid
    Retorna um novo thread id
Parâmetros:
    void
Retorno:
    Se correto:         número da tid
**/
int getNewTid(void);


/**
(Precisamos de uma função pra cada fila que inserirmos porque no escopo estático não é possível chamar de outro arquivo)
putInReadyList
    Bota a thread na fila especificada
Parâmetros:
    PFILA2 List, lista na qual thread será inserida
    TCB_t *thread, thread que será inserida na lista
Retorno:
    Se correto:         devolve 0
    Se erro ocorreu:    devolve < 0
**/
//static int putInList (PFILA2 pointer_list, TCB_t* thread);

int includeInReadyList (TCB_t* thread);
int includeInBlockedList (TCB_t* thread);

/**
getTidFromReadyList
    Pega o tid da fila de aptos
Parâmetros:
    int tid, tid a ser procurada
Retorno:
    Se correto:         devolve TCB_t* da thread
    Se erro ocorreu:    devolve NULL
**/
TCB_t* getTidFromReadyList(int tid);
/**


getTidFromBlockedList
    Pega o tid da fila de bloqueados
Parâmetros:
    int tid, tid a ser procurada
Retorno:
    Se correto:         devolve TCB_t* da thread
    Se erro ocorreu:    devolve NULL
**/
TCB_t* getTidFromBlockedList(int tid);


/**
dispatch
    Usa o escalonador para pegar a próxima tarefa no estado apto
Parâmetros:
    void
Retorno:
    void
**/
void dispatch(void);


/**
blockExecutingThread
    Bota no estado bloqueado a thread que está rodando
Parâmetros:
    void
Retorno:
    Se correto:         devolve 0
    Se erro ocorreu:    devolve < 0
**/
int blockExecutingThread(void);
/**


unblockThread
    Tira no estado bloqueado a thread especificada
Parâmetros:
    void
Retorno:
    Se correto:         devolve 0
    Se erro ocorreu:    devolve < 0
**/
int unblockThread(int tid);


/**
yieldExecutingThread
    Tira voluntariamente de execução thread que está rodando
Parâmetros:
    void
Retorno:
    Se correto:         devolve 0
    Se erro ocorreu:    devolve < 0
**/
int yieldExecutingThread(void);


/**
suspendThread
    Suspende a thread especificada
Parâmetros:
    int tid -> thread id que vai ser suspensa
Retorno:
    Se correto:         devolve 0
    Se erro ocorreu:    devolve < 0
**/
int suspendThread(int tid);


/**
resumeThread
    Resume a thread especificada para apto ou bloqueado
Parâmetros:
    int tid -> thread id que vai ser resumida
Retorno:
    Se correto:         devolve 0
    Se erro ocorreu:    devolve < 0
**/
int resumeThread(int tid);
