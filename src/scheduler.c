#include "../include/support.h"
#include "../include/cdata.h"
#include "../include/scheduler.h"
#include <ucontext.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define STACKMEM 64000

/*O escalonador a ser implementado é do tipo FIFO não preemptivo sem prioridades.
Dessa forma, sempre que uma thread for posta no estado apto, ela será inserida no final da fila de threads aptas a executar.
Quando necessário, o escalonador selecionará para execução a primeira thread da fila*/

//inicialização de variáveis globais gerais
int tid_id = 1;
bool isInitialized = false;

//Filas para representar os estados
static FILA2 filaApto;
static FILA2 filaBloqueado;
static FILA2 filaAptoSuspenso;
static FILA2 filaBloqueadoSuspenso;

//thread que está na CPU
static TCB_t* executing_thread = NULL;

//contexto para liberar memória depois que thread executar
ucontext_t context_to_finish_thread;

void printListsAndExecuting(void){
    FirstFila2(&filaApto);
    FirstFila2(&filaBloqueado);
    FirstFila2(&filaAptoSuspenso);
    FirstFila2(&filaBloqueadoSuspenso);

    if(executing_thread == NULL)
        printf("\n!!nada em execução!!");
    else
        printf("\nEm execução: %d", executing_thread->tid);

    struct sFilaNode2* node;
    TCB_t* threadBeingPrinted = NULL;

    node = GetAtIteratorFila2(&filaApto);
    if(node != NULL)
        threadBeingPrinted = node->node;

    printf("\nFila aptos: ");
    while(threadBeingPrinted != NULL){
        node = GetAtIteratorFila2(&filaApto);
        threadBeingPrinted = node->node;
        printf(" %d",threadBeingPrinted->tid);

        if(NextFila2(&filaApto) == -NXTFILA_ENDQUEUE)
            threadBeingPrinted = NULL;
    }

    node = GetAtIteratorFila2(&filaBloqueado);
    if(node != NULL)
        threadBeingPrinted = node->node;

    printf("\nFila bloqueados: ");
    while(threadBeingPrinted != NULL){
        node = GetAtIteratorFila2(&filaBloqueado);
        threadBeingPrinted = node->node;
        printf(" %d",threadBeingPrinted->tid);

        if(NextFila2(&filaBloqueado) == -NXTFILA_ENDQUEUE)
            threadBeingPrinted = NULL;
    }

    node = GetAtIteratorFila2(&filaAptoSuspenso);
    if(node != NULL)
        threadBeingPrinted = node->node;

    printf("\nFila aptos suspensos: ");
    while(threadBeingPrinted != NULL){
        node = GetAtIteratorFila2(&filaAptoSuspenso);
        threadBeingPrinted = node->node;
        printf(" %d",threadBeingPrinted->tid);

        if(NextFila2(&filaAptoSuspenso) == -NXTFILA_ENDQUEUE)
            threadBeingPrinted = NULL;
    }

    node = GetAtIteratorFila2(&filaBloqueadoSuspenso);
    if(node != NULL)
        threadBeingPrinted = node->node;

    printf("\nFila bloqueados suspensos: ");
    while(threadBeingPrinted != NULL){
        node = GetAtIteratorFila2(&filaBloqueadoSuspenso);
        threadBeingPrinted = node->node;
        printf(" %d",threadBeingPrinted->tid);

        if(NextFila2(&filaBloqueadoSuspenso) == -NXTFILA_ENDQUEUE)
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
ucontext_t* getContextToFinishProcess() {
    return &context_to_finish_thread;
}


/**
getExecutingThread
    devolve a thread em execução
Parâmetros:
    void
Retorno:
    contexto da função de terminar thread
**/
TCB_t* getExecutingThread() {
    return executing_thread;
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
void finishThread(){
    if(executing_thread != NULL){
        if(executing_thread->joinedBeingWaitBy >= 0)
            unblockThread(executing_thread->joinedBeingWaitBy);

        free(executing_thread);
        executing_thread = NULL;

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
void schedulerInitialize(void){
    if(!isInitialized){
    //cria contexto para a função que termina threads
    getcontext(&context_to_finish_thread);
    context_to_finish_thread.uc_link = NULL; //uc_link é pra onde a thread vai depois da execução
    context_to_finish_thread.uc_stack.ss_sp = malloc(STACKMEM);
    context_to_finish_thread.uc_stack.ss_size = STACKMEM;
    context_to_finish_thread.uc_stack.ss_flags = 0;
    makecontext(&context_to_finish_thread, (void *)&finishThread, 0);

    //declara o tcb da thread da main
    TCB_t* main_function = (TCB_t*) malloc(sizeof(TCB_t));
    main_function->tid = 0;
    main_function->state = PROCST_EXEC;
    main_function->prio = 0;
    main_function->context.uc_link = getContextToFinishProcess();

    executing_thread = main_function;

    CreateFila2(&filaApto);
    CreateFila2(&filaBloqueado);
    CreateFila2(&filaBloqueadoSuspenso);
    CreateFila2(&filaAptoSuspenso);
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
int getNewTid(void){
    return tid_id++;
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
static int putInList (PFILA2 pointer_list, TCB_t* thread){
    struct sFilaNode2* nodo = (struct sFilaNode2*) malloc(sizeof(struct sFilaNode2));
    nodo->node = thread;

    int retorno = AppendFila2(pointer_list, nodo);
    printf("\nputting in list");
    printListsAndExecuting();
    return retorno;
}

int includeInReadyList (TCB_t* thread){
    return putInList(&filaApto, thread);
}

int includeInBlockedList (TCB_t* thread){
    return putInList(&filaBloqueado, thread);
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

TCB_t* getTidFromReadyList(int tid){
    FirstFila2(&filaApto);

    struct sFilaNode2* node;
    TCB_t* threadBeingSearched = NULL;

    node = GetAtIteratorFila2(&filaApto);
    if(node != NULL)
        threadBeingSearched = node->node;

    while(threadBeingSearched != NULL && threadBeingSearched->tid != tid){
        node = GetAtIteratorFila2(&filaApto);
        threadBeingSearched = node->node;

        if(NextFila2(&filaApto) == -NXTFILA_ENDQUEUE)
            threadBeingSearched = NULL;
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

TCB_t* getTidFromBlockedList(int tid){
    FirstFila2(&filaBloqueado);

    struct sFilaNode2* node;
    TCB_t* threadBeingSearched = NULL;

    node = GetAtIteratorFila2(&filaBloqueado);
    if(node != NULL)
        threadBeingSearched = node->node;

    while(threadBeingSearched != NULL && threadBeingSearched->tid != tid){
        node = GetAtIteratorFila2(&filaBloqueado);
        threadBeingSearched = node->node;

        if(NextFila2(&filaBloqueado) == -NXTFILA_ENDQUEUE)
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
void dispatch(void){
    printf("\ndispatching");
    struct sFilaNode2* node;
    if(executing_thread == NULL){
        FirstFila2(&filaApto);
        node = GetAtIteratorFila2(&filaApto);
        TCB_t* up_next_thread = node->node;

        if(up_next_thread != NULL){
            up_next_thread->state = PROCST_EXEC;
            up_next_thread->wasJustScheduled = true;
            executing_thread = up_next_thread;

            DeleteAtIteratorFila2(&filaApto);

            setcontext(&(executing_thread->context));
        }
    }
    printListsAndExecuting();
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
int blockExecutingThread(void){
    int status = -1;
    if(executing_thread != NULL){
        executing_thread->state = PROCST_BLOQ;
        status = includeInBlockedList(executing_thread);

        executing_thread = NULL;
    }
    return status;
}

/**
unblockExecutingThread
    Bota no estado bloqueado a thread que está rodando
Parâmetros:
    int tid  -> thread id que vai ser desbloqueada
Retorno:
    Se correto:         devolve 0
    Se erro ocorreu:    devolve < 0
**/
int unblockThread(int tid){
    FirstFila2(&filaBloqueado);
    FirstFila2(&filaBloqueadoSuspenso);

    bool foundInBlocked = false;

    struct sFilaNode2* node;
    TCB_t* threadBeingSearched = NULL;

    node = GetAtIteratorFila2(&filaBloqueado);
    if(node != NULL)
        threadBeingSearched = node->node;

    while(threadBeingSearched != NULL && threadBeingSearched->tid != tid){
        node = GetAtIteratorFila2(&filaBloqueado);
        threadBeingSearched = node->node;

        if(NextFila2(&filaBloqueado) == -NXTFILA_ENDQUEUE)
            threadBeingSearched = NULL;
    }

    if (threadBeingSearched != NULL)
        foundInBlocked = true;

    if(threadBeingSearched == NULL){
        node = GetAtIteratorFila2(&filaBloqueadoSuspenso);
        if(node != NULL)
            threadBeingSearched = node->node;

        while(threadBeingSearched != NULL && threadBeingSearched->tid != tid){
            node = GetAtIteratorFila2(&filaBloqueadoSuspenso);
            threadBeingSearched = node->node;

            if(NextFila2(&filaBloqueadoSuspenso) == -NXTFILA_ENDQUEUE)
                threadBeingSearched = NULL;
        }
    }

    if(threadBeingSearched != NULL && foundInBlocked){
        DeleteAtIteratorFila2(&filaBloqueado);

        threadBeingSearched->state = PROCST_APTO;
        threadBeingSearched->joinedWaitingToFinish = -1;
        putInList(&filaApto, threadBeingSearched);
        return 0;
    }
    else if (threadBeingSearched != NULL){
        DeleteAtIteratorFila2(&filaBloqueadoSuspenso);

        threadBeingSearched->state = PROCST_APTO_SUS;
        threadBeingSearched->joinedWaitingToFinish = -1;
        putInList(&filaAptoSuspenso, threadBeingSearched);
        return 0;
    }


    return -1;
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
int yieldExecutingThread(void){
    int status = -1;
    if(executing_thread != NULL){
        executing_thread->state = PROCST_APTO;
        status = includeInReadyList(executing_thread);

        executing_thread = NULL;
    }
    return status;
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
int suspendThread(int tid){
    FirstFila2(&filaBloqueado);
    FirstFila2(&filaApto);

    bool foundInBlocked = false;

    struct sFilaNode2* node;
    TCB_t* threadBeingSearched = NULL;

    node = GetAtIteratorFila2(&filaBloqueado);
    if(node != NULL)
        threadBeingSearched = node->node;

    while(threadBeingSearched != NULL && threadBeingSearched->tid != tid){
        node = GetAtIteratorFila2(&filaBloqueado);
        threadBeingSearched = node->node;

        if(NextFila2(&filaBloqueado) == -NXTFILA_ENDQUEUE)
            threadBeingSearched = NULL;
    }
    if(threadBeingSearched != NULL)
        foundInBlocked = true;

    if(!foundInBlocked){ //TODO tirar um método dessa função de procurar na lista
        node = GetAtIteratorFila2(&filaApto);
        if(node != NULL)
            threadBeingSearched = node->node;

        while(threadBeingSearched != NULL && threadBeingSearched->tid != tid){
            node = GetAtIteratorFila2(&filaApto);
            threadBeingSearched = node->node;

            if(NextFila2(&filaApto) == -NXTFILA_ENDQUEUE)
                threadBeingSearched = NULL;
        }
        if (threadBeingSearched == NULL)
            return -1;
    }

    if(threadBeingSearched != NULL && foundInBlocked){
        DeleteAtIteratorFila2(&filaBloqueado);

        threadBeingSearched->state = PROCST_BLOQ_SUS;
        putInList(&filaBloqueadoSuspenso, threadBeingSearched);
        return 0;
    }
    else{
        DeleteAtIteratorFila2(&filaApto);

        threadBeingSearched->state = PROCST_APTO_SUS;
        putInList(&filaAptoSuspenso, threadBeingSearched);
        return 0;
    }

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
int resumeThread(int tid){
    FirstFila2(&filaBloqueadoSuspenso);
    FirstFila2(&filaAptoSuspenso);

    bool foundInBlocked = false;

    struct sFilaNode2* node;
    TCB_t* threadBeingSearched = NULL;

    node = GetAtIteratorFila2(&filaBloqueadoSuspenso);
    if(node != NULL)
        threadBeingSearched = node->node;

    while(threadBeingSearched != NULL && threadBeingSearched->tid != tid){
        node = GetAtIteratorFila2(&filaBloqueadoSuspenso);
        threadBeingSearched = node->node;

        if(NextFila2(&filaBloqueadoSuspenso) == -NXTFILA_ENDQUEUE)
            threadBeingSearched = NULL;
    }
    if(threadBeingSearched != NULL)
        foundInBlocked = true;

    if(!foundInBlocked){ //TODO tirar um método dessa função de procurar na lista
        node = GetAtIteratorFila2(&filaAptoSuspenso);
        if(node != NULL)
            threadBeingSearched = node->node;

        while(threadBeingSearched != NULL && threadBeingSearched->tid != tid){
            node = GetAtIteratorFila2(&filaAptoSuspenso);
            threadBeingSearched = node->node;

            if(NextFila2(&filaAptoSuspenso) == -NXTFILA_ENDQUEUE)
                threadBeingSearched = NULL;
        }
        if (threadBeingSearched == NULL)
            return -1;
    }

    if(threadBeingSearched != NULL && foundInBlocked){
        DeleteAtIteratorFila2(&filaBloqueadoSuspenso);

        threadBeingSearched->state = PROCST_BLOQ;
        putInList(&filaBloqueado, threadBeingSearched);
        return 0;
    }
    else if (threadBeingSearched != NULL){
        DeleteAtIteratorFila2(&filaAptoSuspenso);

        threadBeingSearched->state = PROCST_APTO;
        putInList(&filaApto, threadBeingSearched);
        return 0;
    }
    printf("\nnothing found to resume, not found: %d \n", tid);
    return -1;

}


//TODO lógica de semáforos
