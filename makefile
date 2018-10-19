#SD 2018/2019
#Projecto 2 - Grupo 32
#Sandro Correia - 44871
#Diogo Catarino - 44394
#Pedro Almeida - 46401
#*/

SDIR = source
IDIR = include
ODIR = object
BDIR = binary

FLAGS = -Wall -I $(IDIR)


all: clean test_message

clean :
	rm $(ODIR)/*.o $(BDIR)/* $(SDIR)/~*.c ~makefile 2>/dev/null; true

data.o: $(SDIR)/data.c $(IDIR)/data.h
	gcc $(FLAGS) -c $(SDIR)/data.c -o $(ODIR)/data.o

entry.o: $(SDIR)/entry.c $(IDIR)/entry.h
	gcc $(FLAGS) -c $(SDIR)/entry.c -o $(ODIR)/entry.o

list.o: $(SDIR)/list.c $(IDIR)/list.h $(IDIR)/list-private.h
	gcc $(FLAGS) -c $(SDIR)/list.c -o $(ODIR)/list.o

table.o: $(SDIR)/table.c $(IDIR)/table.h $(IDIR)/table-private.h
	gcc $(FLAGS) -c $(SDIR)/table.c -o $(ODIR)/table.o

message.o: $(SDIR)/message.c $(IDIR)/message.h
	gcc $(FLAGS) -c $(SDIR)/message.c -o $(ODIR)/message.o






#regras para os testes da segunda parte do projeto:
test_message.o: $(SDIR)/test_message.c
	gcc $(FLAGS) -c $(SDIR)/test_message.c -o $(ODIR)/test_message.o

test_message: test_message.o message.o data.o table.o entry.o list.o
	gcc $(ODIR)/test_message.o $(ODIR)/message.o $(ODIR)/data.o $(ODIR)/table.o $(ODIR)/entry.o $(ODIR)/list.o -o $(BDIR)/test_message