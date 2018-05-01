#
# Makefile para programas exemplo
# 

CC=gcc
LIB_DIR=../lib

all: exemplo mandel teste_vetor series teste_semaforo_e_escalonador

exemplo: exemplo.c $(LIB_DIR)/cthread.a
	$(CC) -o exemplo exemplo.c $(LIB_DIR)/cthread.a -Wall

teste_vetor: teste_vetor.c $(LIB_DIR)/cthread.a
	$(CC) -o teste_vetor teste_vetor.c $(LIB_DIR)/cthread.a -Wall

mandel: mandel.c $(LIB_DIR)/cthread.a
	$(CC) -o mandel mandel.c $(LIB_DIR)/cthread.a -lm -Wall

#barbeiro: barbeiro.c $(LIB_DIR)/libcthread.a
#	$(CC) -o barbeiro barbeiro.c -L$(LIB_DIR) -lcthread -lm -Wall

#filosofos: filosofos.c $(LIB_DIR)/libcthread.a
#	$(CC) -o filosofos filosofos.c -L$(LIB_DIR) -lcthread -lm -Wall

#prodcons: prodcons.c $(LIB_DIR)/libcthread.a
#	$(CC) -o prodcons prodcons.c -L$(LIB_DIR) -lcthread -lm -Wall

series: series.c $(LIB_DIR)/cthread.a
	$(CC) -o series series.c $(LIB_DIR)/cthread.a -lm -Wall

teste_semaforo_e_escalonador: teste_semaforo_e_escalonador.c $(LIB_DIR)/cthread.a
	$(CC) -o teste_semaforo_e_escalonador teste_semaforo_e_escalonador.c $(LIB_DIR)/cthread.a -lm -Wall
	
main: main.c $(LIB_DIR)/cthread.a
	$(CC) -o exemplo main.c $(LIB_DIR)/cthread.a -Wall

clean:
	rm -rf exemplo mandel teste_vetor barbeiro filosofos prodcons series *.o *~




