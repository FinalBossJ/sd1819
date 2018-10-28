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

CC = gcc

deps_test_message = $(ODIR)/test_message.o $(ODIR)/data.o $(ODIR)/entry.o $(ODIR)/table.o $(ODIR)/list.o $(ODIR)/message.o 
deps_table-client = $(ODIR)/table-client.o $(ODIR)/network_client.o $(ODIR)/data.o $(ODIR)/entry.o $(ODIR)/table.o $(ODIR)/list.o $(ODIR)/message.o $(ODIR)/client_stub.o 
deps_table-server = $(ODIR)/table-server.o $(ODIR)/network_server.o $(ODIR)/table_skel.o $(ODIR)/data.o $(ODIR)/entry.o $(ODIR)/table.o $(ODIR)/list.o $(ODIR)/message.o 

all: make_folders clean test_message table-client table-server


table-client : $(BDIR)/table-client

$(BDIR)/table-client : $(deps_table-client)
	$(CC) -o $@ $^

table-server : $(BDIR)/table-server

$(BDIR)/table-server : $(deps_table-server)
	$(CC) -o $@ $^


test_message : $(BDIR)/test_message

$(BDIR)/test_message : $(deps_test_message)
	$(CC) -o $@ $^

$(ODIR)/%.o : $(SDIR)/%.c
	$(CC) -c $< -o $@ $(FLAGS)

make_folders :
	mkdir -p object; mkdir -p binary

clean :
	rm $(ODIR)/*.o $(BDIR)/* $(SDIR)/~*.c ~makefile 2>/dev/null; true