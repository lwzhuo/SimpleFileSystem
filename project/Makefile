OUTDIR=./out

fs:main.o function.o disk.o list.o shell.o str.o time.o
	cc -o fs $(OUTDIR)/main.o $(OUTDIR)/function.o $(OUTDIR)/disk.o $(OUTDIR)/list.o $(OUTDIR)/shell.o $(OUTDIR)/str.o $(OUTDIR)/time.o

main.o:main.c ./function/api.h ./shell/shell.h
	cc -c main.c 
	mv main.o $(OUTDIR)/main.o

function.o:./function/function.c ./function/api.h
	cc -c ./function/function.c
	mv function.o $(OUTDIR)/function.o

disk.o:./util/disk.c ./util/disk.h
	cc -c ./util/disk.c
	mv disk.o $(OUTDIR)/disk.o

list.o:./util/list.c ./util/list.h
	cc -c ./util/list.c
	mv list.o $(OUTDIR)/list.o

shell.o:./shell/shell.c ./shell/shell.h
	cc -c ./shell/shell.c
	mv shell.o $(OUTDIR)/shell.o

str.o:./util/str.c ./util/str.h
	cc -c ./util/str.c
	mv str.o $(OUTDIR)/str.o

time.o:./util/time.c ./util/time.h
	cc -c ./util/time.c
	mv time.o $(OUTDIR)/time.o
.PHONY:clean
clean:
	-rm $(OUTDIR)/main.o $(OUTDIR)/function.o $(OUTDIR)/disk.o $(OUTDIR)/list.o $(OUTDIR)/shell.o $(OUTDIR)/str.o