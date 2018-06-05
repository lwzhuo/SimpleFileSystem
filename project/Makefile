OUTDIR=./out

fs:main.o function.o disk.o 
	cc -o fs $(OUTDIR)/main.o $(OUTDIR)/function.o $(OUTDIR)/disk.o

main.o:main.c ./function/api.h
	cc -c main.c 
	mv main.o $(OUTDIR)/main.o

function.o:./function/function.c ./function/api.h
	cc -c ./function/function.c
	mv function.o $(OUTDIR)/function.o

disk.o:./util/disk.c ./util/disk.h
	cc -c ./util/disk.c
	mv disk.o $(OUTDIR)/disk.o

.PHONY:clean
clean:
	-rm $(OUTDIR)/function.o $(OUTDIR)/main.o $(OUTDIR)/disk.o