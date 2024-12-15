# Nomes dos executáveis
EXEC_1 = teste_mqueue
EXEC_2 = teste_semaphore
EXEC_3 = teste_racecond

# Arquivos de objeto e fonte comuns
COMMON_OBJS = ppos-core-aux.c ppos-all.o queue.o

# Compilador e flags
CC = gcc
CFLAGS = -lm

# Regras principais
all: $(EXEC_1) $(EXEC_2) $(EXEC_3)

# Regras específicas para cada versão do programa
$(EXEC_1): $(COMMON_OBJS) pingpong-mqueue.c
	$(CC) -o $(EXEC_1) $(COMMON_OBJS) pingpong-mqueue.c $(CFLAGS)

$(EXEC_2): $(COMMON_OBJS) pingpong-semaphore.c
	$(CC) -o $(EXEC_2) $(COMMON_OBJS) pingpong-semaphore.c

$(EXEC_3): $(COMMON_OBJS) pingpong-racecond.c
	$(CC) -o $(EXEC_3) $(COMMON_OBJS) pingpong-racecond.c

# Limpeza dos arquivos gerados
clean:
	rm -f $(EXEC_1) $(EXEC_2) $(EXEC_3)

