SDIR = source
IDIR = include
ODIR = object

FLAGS = -Wall -I $(IDIR)


tests: test_data test_entry test_list test_table


data.o: $(SDIR)/data.c $(IDIR)/data.h
	gcc $(FLAGS) -c $(SDIR)/data.c -o $(ODIR)/data.o

entry.o: $(SDIR)/entry.c $(IDIR)/entry.h
	gcc $(FLAGS) -c $(SDIR)/entry.c -o $(ODIR)/entry.o

list.o: $(SDIR)/list.c $(IDIR)/list.h $(IDIR)/list-private.h
	gcc $(FLAGS) -c $(SDIR)/list.c -o $(ODIR)/list.o

table.o: $(SDIR)/table.c $(IDIR)/table.h $(IDIR)/table-private.h
	gcc $(FLAGS) -c $(SDIR)/table.c -o $(ODIR)/table.o




#regras para os testes da primeira parte do projeto:
test_data.o: $(SDIR)/test_data.c
	gcc $(FLAGS) -c $(SDIR)/test_data.c -o $(ODIR)/test_data.o

test_entry.o: $(SDIR)/test_entry.c
	gcc $(FLAGS) -c $(SDIR)/test_entry.c -o $(ODIR)/test_entry.o

test_list.o: $(SDIR)/test_list.c
	gcc $(FLAGS) -c $(SDIR)/test_list.c -o $(ODIR)/test_list.o

test_table.o: $(SDIR)/test_table.c
	gcc $(FLAGS) -c $(SDIR)/test_table.c -o $(ODIR)/test_table.o

test_table: test_table.o table.o list.o entry.o data.o
	gcc $(ODIR)/test_table.o $(ODIR)/table.o $(ODIR)/list.o $(ODIR)/entry.o $(ODIR)/data.o -o test_table

test_data: test_data.o data.o
	gcc $(ODIR)/test_data.o $(ODIR)/data.o -o test_data

test_entry: test_entry.o entry.o data.o
	gcc $(ODIR)/test_entry.o $(ODIR)/entry.o $(ODIR)/data.o -o test_entry

test_list: test_list.o list.o entry.o data.o
	gcc $(ODIR)/test_list.o $(ODIR)/list.o $(ODIR)/entry.o $(ODIR)/data.o -o test_list