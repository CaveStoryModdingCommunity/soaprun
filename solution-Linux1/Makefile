CC      = gcc
TARGET  = App
LDFLAGS = -lstdc++ -lpthread

OBJS    =			\
	main.o			\
	Daemon.o		\
	Directory.o		\
	DebugLog.o		\
	lnxSock.o		\
	Loop_Main.o		\
	Signal.o		\
	svAttender.o	\
	svListener.o	\
	test_Server.o	\
	test_Thread.o
	

$(TARGET):$(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

.PHONY: run clean version daemon
	
run:
	./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJS)

version :
	./$(TERGET) --version
	
daemon  :
	./$(TERGET) --daemon
