OUTDIR=./out

fs:main.o function.o
	cc -o fs $(OUTDIR)/main.o $(OUTDIR)/function.o

main.o:main.c ./function/api.h
	cc -c main.c 
	mv main.o $(OUTDIR)/main.o

function.o:./function/function.c ./global/var.h ./function/api.h 
	cc -c ./function/function.c 
	mv function.o $(OUTDIR)/function.o

clean:
	rm $(OUTDIR)/function.o $(OUTDIR)/main.o