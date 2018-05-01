#include "../include/support.h"
#include "../include/cdata.h"
#include "../include/scheduler.h"
#include <ucontext.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

/*O escalonador a ser implementado é do tipo FIFO não preemptivo sem prioridades.
Dessa forma, sempre que uma thread for posta no estado apto, ela será inserida no final da fila de threads aptas a executar.
Quando necessário, o escalonador selecionará para execução a primeira thread da fila*/

//inicialização de variáveis globais gerais
int tidId = 1;
bool isInitialized = false;

//Filas para representar os estados
static FILA2 readyList;
static FILA2 blockedList;
static FILA2 readySuspendedList;
static FILA2 blockedSuspendedList;

//thread que está na CPU
static TCB_t* executingThread = NULL;

//contexto para liberar memória depois que thread executar
ucontext_t contextToFinishThread;

void printListsAndExecuting(void) {
    if(!isInitialized) {
        schedulerInitialize();
    }

    FirstFila2(&readyList);
    FirstFila2(&blockedList);
    FirstFila2(&readySuspendedList);
    FirstFila2(&blockedSuspendedList);

    if(executingThread == NULL) {
        printf("\n!!nada em execução!!");
    }
    else {
        printf("\nEm execução: %d", executingThread->tid);
    }

    struct sFilaNode2* node;
    TCB_t* threadBeingPrinted = NULL;

    node = GetAtIteratorFila2(&readyList);
    if(node != NULL) {
        threadBeingPrinted = node->node;
    }

    printf("\nFila aptos: ");
    while(threadBeingPrinted != NULL) {
        node = GetAtIteratorFila2(&readyList);
        threadBeingPrinted = node->node;
        printf(" %d",threadBeingPrinted->tid);

        if(NextFila2(&readyList) == -NXTFILA_ENDQUEUE) {
            threadBeingPrinted = NULL;
        }
    }

    node = GetAtIteratorFila2(&blockedList);
    if(node != NULL) {
        threadBeingPrinted = node->node;
    }

    printf("\nFila bloqueados: ");
    while(threadBeingPrinted != NULL) {
        node = GetAtIteratorFila2(&blockedList);
        threadBeingPrinted = node->node;
        printf(" %d",threadBeingPrinted->tid);

        if(NextFila2(&blockedList) == -NXTFILA_ENDQUEUE) {
            threadBeingPrinted = NULL;
        }
    }

    node = GetAtIteratorFila2(&readySuspendedList);
    if(node != NULL) {
        threadBeingPrinted = node->node;
    }

    printf("\nFila aptos suspensos: ");
    while(threadBeingPrinted != NULL) {
        node = GetAtIteratorFila2(&readySuspendedList);
        threadBeingPrinted = node->node;
        printf(" %d",threadBeingPrinted->tid);

        if(NextFila2(&readySuspendedList) == -NXTFILA_ENDQUEUE) {
            threadBeingPrinted = NULL;
        }
    }

    node = GetAtIteratorFila2(&blockedSuspendedList);
    if(node != NULL) {
        threadBeingPrinted = node->node;
    }

    printf("\nFila bloqueados suspensos: ");
    while(threadBeingPrinted != NULL) {
        node = GetAtIteratorFila2(&blockedSuspendedList);
        threadBeingPrinted = node->node;
        printf(" %d",threadBeingPrinted->tid);

        if(NextFila2(&blockedSuspendedList) == -NXTFILA_ENDQUEUE)
            threadBeingPrinted = NULL;
    }
}

/**
getContextToFinishProcess
    devolve um contexto qu9e vai estar atrelado a uma função que termina a execução da thread
Parâmetros:
    void
Retorno:
    contexto da função de terminar thread
**/
ucontext_t* getContextToFinishProcess()
{
    return &contextToFinishThread;
}


/**
getExecutingThread
    devolve a thread em execução
Parâmetros:
    void
Retorno:
    contexto da função de terminar thread
**/
TCB_t* getExecutingThread()
{
    if(!isInitialized) {
        schedulerInitialize();
    }

    return executingThread;
}


/**
finishThread
    termina a thread e libera a memória que ela ocupava
Parâmetros:
    TCB_t* thread, a thread a ser liberada da memória
Retorno:
    Se correto:         devolve 0
    Se erro ocorreu:    devolve < 0
**/
void finishThread()
{
    if(!isInitialized) {
        schedulerInitialize();
    }

    if(executingThread != NULL) {
        if(executingThread->joinedBeingWaitBy >= 0)
            unblockThread(executingThread->joinedBeingWaitBy);

        free(executingThread);
        executingThread = NULL;

        dispatch();
    }
}


/**
schedulerInitialize
    Inicializa todas filas
Parâmetros:
    void
Retorno:
    void
**/
void schedulerInitialize(void)
{
    if(!isInitialized) {
        //cria contexto para a função que termina threads
        getcontext(&contextToFinishThread);
        contextToFinishThread.uc_link = NULL; //uc_link é pra onde a thread vai depois da execução
        contextToFinishThread.uc_stack.ss_sp = (char*) malloc(SIGSTKSZ);
        contextToFinishThread.uc_stack.ss_size = SIGSTKSZ;
        contextToFinishThread.uc_stack.ss_flags = 0;
        makecontext(&contextToFinishThread, (void *)&finishThread, 0);

        //declara o tcb da thread da main
        TCB_t* mainFunction = (TCB_t*) malloc(sizeof(TCB_t));
        mainFunction->tid = 0;
        mainFunction->state = PROCST_EXEC;
        mainFunction->prio = 0;
        mainFunction->context.uc_link = getContextToFinishProcess();

        executingThread = mainFunction;

        CreateFila2(&readyList);
        CreateFila2(&blockedList);
        CreateFila2(&blockedSuspendedList);
        CreateFila2(&readySuspendedList);
        isInitialized = true;
    }
}


/**
getNewTid
    Retorna um novo thread id
Parâmetros:
    void
Retorno:
    novo número tid
**/
int getNewTid(void)
{
    if(!isInitialized) {
        schedulerInitialize();
    }

    return tidId++;
}


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
static int putInList (PFILA2 pointerList, TCB_t* thread)
{
    if(!isInitialized) {
        schedulerInitialize();
    }

    struct sFilaNode2* nodo = (struct sFilaNode2*) malloc(sizeof(struct sFilaNode2));
    nodo->node = thread;

    int status = AppendFila2(pointerList, nodo);

    if(status < 0) {
        return ERROR;
    }
    else {
        return SUCCESS;
    }
}

int includeInReadyList (TCB_t* thread)
{
    if(!isInitialized) {
        schedulerInitialize();
    }

    return putInList(&readyList, thread);
}

int includeInBlockedList (TCB_t* thread)
{
    if(!isInitialized) {
        schedulerInitialize();
    }

    return putInList(&blockedList, thread);
}

bool isReadyListEmpty()
{
    if(!isInitialized) {
        schedulerInitialize();
    }

    return FirstFila2(&readyList) != 0 ? true : false;
}

/**
getTidFromReadyList
    Pega o tid da fila de aptos
Parâmetros:
    int tid, tid a ser procurada
Retorno:
    Se correto:         devolve TCB_t* da thread
    Se erro ocorreu:    devolve NULL
**/

TCB_t* getTidFromReadyList(int tid)
{
    if(!isInitialized) {
        schedulerInitialize();
    }
    FirstFila2(&readyList);

    struct sFilaNode2* node;
    TCB_t* threadBeingSearched = NULL;
    node = GetAtIteratorFila2(&readyList);
    if(node != NULL) {
        threadBeingSearched = node->node;
    }

    while(threadBeingSearched != NULL && threadBeingSearched->tid != tid) {
        node = GetAtIteratorFila2(&readyList);
        threadBeingSearched = node->node;

        if(NextFila2(&readyList) == -NXTFILA_ENDQUEUE) {
            threadBeingSearched = NULL;
        }
    }

    return threadBeingSearched;
}

/**
getTidFromBlockedList
    Pega o tid da fila de bloqueados
Parâmetros:
    int tid, tid a ser procurada
Retorno:
    Se correto:         devolve TCB_t* da thread
    Se erro ocorreu:    devolve NULL
**/

TCB_t* getTidFromBlockedList(int tid)
{
    if(!isInitialized) {
        schedulerInitialize();
    }
    FirstFila2(&blockedList);

    struct sFilaNode2* node;
    TCB_t* threadBeingSearched = NULL;
    node = GetAtIteratorFila2(&blockedList);
    if(node != NULL) {
        threadBeingSearched = node->node;
    }

    while(threadBeingSearched != NULL && threadBeingSearched->tid != tid) {
        node = GetAtIteratorFila2(&blockedList);
        threadBeingSearched = node->node;

        if(NextFila2(&blockedList) == -NXTFILA_ENDQUEUE)
            threadBeingSearched = NULL;
    }

    return threadBeingSearched;
}


/**
dispatch
    Usa o escalonador para pegar a próxima tarefa no estado apto
Parâmetros:
    void
Retorno:
    void
**/
void dispatch(void)
{
    if(!isInitialized) {
        schedulerInitialize();
    }

    struct sFilaNode2* node;
    if(executingThread == NULL) {
        FirstFila2(&readyList);
        node = GetAtIteratorFila2(&readyList);
        TCB_t* upNextThread = node->node;

        if(upNextThread != NULL) {
            upNextThread->state = PROCST_EXEC;
            upNextThread->wasJustScheduled = true;
            executingThread = upNextThread;

            DeleteAtIteratorFila2(&readyList);

            setcontext(&(executingThread->context));
        }
    }
}


/**
blockExecutingThread
    Bota no estado bloqueado a thread que está rodando
Parâmetros:
    void
Retorno:
    Se correto:         devolve 0
    Se erro ocorreu:    devolve < 0
**/
int blockExecutingThread(void) {
    if(!isInitialized) {
        schedulerInitialize();
    }

    int status = -1;
    if(executingThread != NULL) {
        TCB_t* blockedThread = executingThread;
        executingThread = NULL;

        blockedThread->state = PROCST_BLOQ;
        status = includeInBlockedList(blockedThread);
    }

    if(status < 0) {
        return ERROR;
    }
    else {
        return SUCCESS;
    }
}

/**
unblockThread
    Bota no estado bloqueado a thread que está rodando
Parâmetros:
    int tid  -> thread id que vai ser desbloqueada
Retorno:
    Se correto:         devolve 0
    Se erro ocorreu:    devolve < 0
**/
int unblockThread(int tid) {
    if(!isInitialized) {
        schedulerInitialize();
    }

    FirstFila2(&blockedList);
    FirstFila2(&blockedSuspendedList);

    bool foundInBlocked = false;

    struct sFilaNode2* node;
    TCB_t* threadBeingSearched = NULL;

    node = GetAtIteratorFila2(&blockedList);
    if(node != NULL) {
        threadBeingSearched = node->node;
    }

    while(threadBeingSearched != NULL && threadBeingSearched->tid != tid) {
        node = GetAtIteratorFila2(&blockedList);
        threadBeingSearched = node->node;

        if(NextFila2(&blockedList) == -NXTFILA_ENDQUEUE) {
            threadBeingSearched = NULL;
        }
    }

    if (threadBeingSearched != NULL) {
        foundInBlocked = true;
    }

    if(threadBeingSearched == NULL) {
        node = GetAtIteratorFila2(&blockedSuspendedList);
        if(node != NULL) {
            threadBeingSearched = node->node;
        }

        while(threadBeingSearched != NULL && threadBeingSearched->tid != tid) {
            node = GetAtIteratorFila2(&blockedSuspendedList);
            threadBeingSearched = node->node;

            if(NextFila2(&blockedSuspendedList) == -NXTFILA_ENDQUEUE) {
                threadBeingSearched = NULL;
            }
        }
    }

    if(threadBeingSearched != NULL && foundInBlocked) {
        DeleteAtIteratorFila2(&blockedList);

        threadBeingSearched->state = PROCST_APTO;
        threadBeingSearched->joinedWaitingToFinish = -1;
        putInList(&readyList, threadBeingSearched);
        return SUCCESS;
    }
    else if (threadBeingSearched != NULL) {
        DeleteAtIteratorFila2(&blockedSuspendedList);

        threadBeingSearched->state = PROCST_APTO_SUS;
        threadBeingSearched->joinedWaitingToFinish = -1;
        putInList(&readySuspendedList, threadBeingSearched);
        return SUCCESS;
    }

    return ERROR;
}


/**
yieldExecutingThread
    Tira voluntariamente de execução thread que está rodando
Parâmetros:
    void
Retorno:
    Se correto:         devolve 0
    Se erro ocorreu:    devolve < 0
**/
int yieldExecutingThread(void) {
    if(!isInitialized) {
        schedulerInitialize();
    }

    int status = -1;
    if(executingThread != NULL) {
        TCB_t* yieldedThread = executingThread;
        executingThread = NULL;

        yieldedThread->state = PROCST_APTO;
        status = includeInReadyList(yieldedThread);
    }

    if(status < 0) {
        return ERROR;
    }
    else {
        return SUCCESS;
    }
}


/**
suspendThread
    Suspende a thread especificada
Parâmetros:
    int tid -> thread id que vai ser suspensa
Retorno:
    Se correto:         devolve 0
    Se erro ocorreu:    devolve < 0
**/
int suspendThread(int tid) {
    if(!isInitialized) {
        schedulerInitialize();
    }

    FirstFila2(&blockedList); //Deveria ser a fila de bloqueados suspensos e nao apenas bloqueado
    FirstFila2(&readyList);

    bool foundInBlocked = false;

    struct sFilaNode2* node;
    TCB_t* threadBeingSearched = NULL;

    node = GetAtIteratorFila2(&blockedList);
    if(node != NULL) {
        threadBeingSearched = node->node;
    }

    while(threadBeingSearched != NULL && threadBeingSearched->tid != tid) {
        node = GetAtIteratorFila2(&blockedList);
        threadBeingSearched = node->node;

        if(NextFila2(&blockedList) == -NXTFILA_ENDQUEUE) {
            threadBeingSearched = NULL;
        }
    }
    if(threadBeingSearched != NULL) {
        foundInBlocked = true;
    }

    if(!foundInBlocked) {
        node = GetAtIteratorFila2(&readyList);
        if(node != NULL)
            threadBeingSearched = node->node;

        while(threadBeingSearched != NULL && threadBeingSearched->tid != tid) {
            node = GetAtIteratorFila2(&readyList);
            threadBeingSearched = node->node;

            if(NextFila2(&readyList) == -NXTFILA_ENDQUEUE){
                threadBeingSearched = NULL;
            }
        }
        if (threadBeingSearched == NULL) {
            return ERROR;
        }
    }

    if(threadBeingSearched != NULL && foundInBlocked) {
        DeleteAtIteratorFila2(&blockedList);

        threadBeingSearched->state = PROCST_BLOQ_SUS;
        putInList(&blockedSuspendedList, threadBeingSearched);
        return SUCCESS;
    }
    else{
        DeleteAtIteratorFila2(&readyList);

        threadBeingSearched->state = PROCST_APTO_SUS;
        putInList(&readySuspendedList, threadBeingSearched);
        return SUCCESS;
    }

    return ERROR;
}


/**
resumeThread
    Resume a thread especificada para apto ou bloqueado
Parâmetros:
    int tid -> thread id que vai ser resumida
Retorno:
    Se correto:         devolve 0
    Se erro ocorreu:    devolve < 0
**/
int resumeThread(int tid) {
    if(!isInitialized) {
        schedulerInitialize();
    }

    FirstFila2(&blockedSuspendedList);
    FirstFila2(&readySuspendedList);

    bool foundInBlocked = false;

    struct sFilaNode2* node;
    TCB_t* threadBeingSearched = NULL;

    node = GetAtIteratorFila2(&blockedSuspendedList);
    if(node != NULL) {
        threadBeingSearched = node->node;
    }

    while(threadBeingSearched != NULL && threadBeingSearched->tid != tid) {
        node = GetAtIteratorFila2(&blockedSuspendedList);
        threadBeingSearched = node->node;

        if(NextFila2(&blockedSuspendedList) == -NXTFILA_ENDQUEUE) {
            threadBeingSearched = NULL;
        }
    }
    if(threadBeingSearched != NULL) {
        foundInBlocked = true;
    }

    if(!foundInBlocked) {
        node = GetAtIteratorFila2(&readySuspendedList);
        if(node != NULL) {
            threadBeingSearched = node->node;
        }

        while(threadBeingSearched != NULL && threadBeingSearched->tid != tid) {
            node = GetAtIteratorFila2(&readySuspendedList);
            threadBeingSearched = node->node;

            if(NextFila2(&readySuspendedList) == -NXTFILA_ENDQUEUE) {
                threadBeingSearched = NULL;
            }
        }
        if (threadBeingSearched == NULL) {
            return ERROR;
        }
    }

    if(threadBeingSearched != NULL && foundInBlocked) {
        DeleteAtIteratorFila2(&blockedSuspendedList);

        threadBeingSearched->state = PROCST_BLOQ;
        putInList(&blockedList, threadBeingSearched);
        return SUCCESS;
    }
    else if (threadBeingSearched != NULL) {
        DeleteAtIteratorFila2(&readySuspendedList);

        threadBeingSearched->state = PROCST_APTO;
        putInList(&readyList, threadBeingSearched);
        return SUCCESS;
    }

    return ERROR;
}
