CC = gcc  #编译器
CONFIG = `pkg-config --cflags --libs gtk+-2.0`  #相关配置
ALL = main.o ui.o signalAction.o files.o #相关依懒
OBJ = liang  #最终生成目标

$(OBJ):$(ALL)
	$(CC) -g -I. -L. $(ALL) -o $(OBJ) $(CONFIG)
	
main.o:main.c
	$(CC) -g -c main.c $(CONFIG)

ui.o:ui.c ui.h
	$(CC) -g -I. -c ui.h ui.c $(CONFIG)

signalAction.o:signalAction.c signalAction.h
	$(CC) -g -I. -c signalAction.c signalAction.h $(CONFIG)

files.o:files.h files.c
	$(CC) -g -I. -c files.c $(CONFIG)
	
clean:
	rm *.o *.gch

