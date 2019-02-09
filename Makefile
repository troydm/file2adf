CFLAGS=-O2 -I/usr/local/lib/adflib/include
LDFLAGS=-L/usr/local/lib/adflib/lib -ladf

file2adf: file2adf.c
	cc $(CFLAGS) -c file2adf.c
	cc -o file2adf file2adf.o $(LDFLAGS)

clean:
	rm -f file2adf file2adf.o
