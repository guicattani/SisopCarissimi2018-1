
/********************************************************************

	Support Library
	Vers.2018.1 - 20/03/18
	
********************************************************************/

#ifndef	__SUPPORTE_H__
#define	__SUPPORTE_H__

struct	sFilaNode2 {
	void 	*node;			// Ponteiro para a estrutura de dados do NODO
	struct	sFilaNode2 *ant;	// Ponteiro para o nodo anterior
	struct	sFilaNode2 *next;	// Ponteiro para o nodo posterior
};
struct sFila2 {
	struct	sFilaNode2 *it;		// Iterador para varrer a lista
	struct	sFilaNode2 *first;	// Primeiro elemento da lista
	struct	sFilaNode2 *last;	// Último elemento da lista
};

typedef struct sFilaNode2	NODE2;
typedef struct sFila2		FILA2;
typedef struct sFilaNode2 *	PNODE2;
typedef struct sFila2 *		PFILA2;

/*-------------------------------------------------------------------
Função:	Informa a versao da biblioteca
Ret:	Numero da versao
-------------------------------------------------------------------*/
#define Year	2018
#define	Term	1
#define Version()	((2*Year)+(Term-1))


/*-------------------------------------------------------------------
Função:	Inicializa uma estrutura de dados do tipo FILA2
Ret:	==0, se conseguiu
	!=0, caso contrário (erro ou fila vazia)
-------------------------------------------------------------------*/
int	CreateFila2(PFILA2 pFila);


/*-------------------------------------------------------------------
Função:	Seta o iterador da fila no primeiro elemento
Ret:	==0, se conseguiu
	!=0, caso contrário (erro ou fila vazia)
-------------------------------------------------------------------*/
int	FirstFila2(PFILA2 pFila);


/*-------------------------------------------------------------------
Função:	Seta o iterador da fila no último elemento
Ret:	==0, se conseguiu
	!=0, caso contrário (erro ou fila vazia)
-------------------------------------------------------------------*/
int	LastFila2(PFILA2 pFila);


/*-------------------------------------------------------------------
Função:	Seta o iterador da fila para o próximo elemento
Ret:	==0, se conseguiu
	!=0, caso contrário (erro, fila vazia ou chegou ao final da fila)
	Fila vazia		=> -NXTFILA_VAZIA
	Iterador inválido	=> -NXTFILA_ITERINVAL
	Atingido final da fila	=> -NXTFILA_ENDQUEUE
-------------------------------------------------------------------*/
#define	NXTFILA_VAZIA		1
#define	NXTFILA_ITERINVAL	2
#define	NXTFILA_ENDQUEUE	3
int	NextFila2(PFILA2 pFila);


/*-------------------------------------------------------------------
Função:	Retorna o conteúdo do nodo endereçado pelo iterador da lista "pFila"
Ret:	Ponteiro válido, se conseguiu
	NULL, caso contrário:
		-> first==NULL (lista vazia)
		-> it==NULL (iterador invalido)
-------------------------------------------------------------------*/
void	*GetAtIteratorFila2(PFILA2 pFila);


/*-------------------------------------------------------------------
Função:	Retorna o conteúdo do nodo endereçado pelo iterador->next da lista "pFila"
Ret:	Ponteiro válido, se conseguiu
	NULL, caso contrário:
		-> first==NULL (lista vazia)
		-> it==NULL (iterador invalido)
		-> it->next==NULL (não tem NEXT)
-------------------------------------------------------------------*/
void 	*GetAtNextIteratorFila2(PFILA2 pFila);


/*-------------------------------------------------------------------
Função:	Retorna o conteúdo do nodo endereçado pelo iterador->ant da lista "pFila"
Ret:	Ponteiro válido, se conseguiu
	NULL, caso contrário:
		-> first==NULL (lista vazia)
		-> it==NULL (iterador invalido)
		-> it->ant==NULL (não tem ANT)
-------------------------------------------------------------------*/
void 	*GetAtAntIteratorFila2(PFILA2 pFila);


/*-------------------------------------------------------------------
Função:	Coloca o ponteiro "content" no final da fila "pFila"
Ret:	==0, se conseguiu
	!=0, caso contrário (erro)
-------------------------------------------------------------------*/
int	AppendFila2(PFILA2 pFila, void *content);


/*-------------------------------------------------------------------
Função:	Coloca o ponteiro "content" logo após o elemento 
	correntemente apontado pelo iterador da fila "pFila"
Ret:	==0, se conseguiu
	!=0, caso contrário (erro)
	Fila vazia		=> -INSITER_VAZIA
	Iterador inválido	=> -INSITER_INVAL
-------------------------------------------------------------------*/
#define	INSITER_VAZIA	1
#define	INSITER_INVAL	2
int	InsertAfterIteratorFila2(PFILA2 pFila, void *content);


/*-------------------------------------------------------------------
Função:	Coloca o ponteiro "content" logo antes do elemento correntemente apontado pelo iterador da fila "pFila"
	A fila tem que ter, pelo menos, um elemento.
	O iterador tem que ser válido.
	Se a fila estiver vazia deve-se usar "AppendFila2"
Ret:	==0, se conseguiu
	!=0, caso contrário (erro)
	Fila vazia		=> -INSITER_VAZIA
	Iterador inválido	=> -INSITER_INVAL
-------------------------------------------------------------------*/
int	InsertBeforeIteratorFila2(PFILA2 pFila, void *content);


/*-------------------------------------------------------------------
Função:	Remove o elemento indicado pelo iterador, da lista "pFila"
Ret:	==0, se conseguiu
	!=0, caso contrário (erro)
	Fila vazia		=> -DELITER_VAZIA
	Iterador inválido	=> -DELITER_INVAL
-------------------------------------------------------------------*/
#define	DELITER_VAZIA	1
#define	DELITER_INVAL	2
int	DeleteAtIteratorFila2(PFILA2 pFila);


/*-------------------------------------------------------------------
Função:	Gera um número pseudo-aleatório entre 0 e 65535
Ret:	Número gerado
-------------------------------------------------------------------*/
unsigned int	Random2();


/*-------------------------------------------------------------------
Função:	Dispara a leitura do delay de tempo
-------------------------------------------------------------------*/
void	startTimer();


/*-------------------------------------------------------------------
Função:	Encerra a leitura do timer e informa o tempo transcorrido
	O valor retornado está um número de ciclos de clock da CPU
Ret:	Diferença de "tempo" entre o startTimer() e o stopTimer()
-------------------------------------------------------------------*/
unsigned int stopTimer();



#endif




