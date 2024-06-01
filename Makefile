CC 	= gcc
CFLAGS 	= 
EXEC	= tush
INCLS	= 
LIBS	=

OBJS = tush.o inarg.o internalcommand.o runcommand.o

$(EXEC): $(OBJS)
	$(CC)  $(CFLAGS) -o $(EXEC) $(OBJS) $(LIBS)

$(OBJS):
	$(CC)  $(CFLAGS)  $(INCLS)  -c  $*.c

clean:
	@echo "cleaning ..."
	@/bin/rm *.o $(EXEC)


