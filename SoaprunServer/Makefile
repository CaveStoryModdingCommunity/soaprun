CC      = gcc
TARGET  = SoaprunServer
LDFLAGS = -lstdc++ -lpthread

OBJS    =			\
	main.o			\
	Daemon.o		\
	Directory.o		\
	DebugLog.o		\
	dt_FldObj.o		\
	dt_MapData.o	\
	LinkUnits.o		\
	lnxSock.o		\
	mPrtsHistory.o	\
	NPU_MapGrid.o	\
	SharedDefines.o	\
	Signal.o		\
	svAttender.o	\
	svFldUnit.o		\
	svFieldUnits.o	\
	svListener.o	\
	svPosHistory.o
	

$(TARGET):$(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

.PHONY: run run2 clean version daemon
	
run:
	./$(TARGET)
	
run2:
	./$(TARGET) --port 1002

clean:
	rm -f $(TARGET) $(OBJS)

version :
	./$(TERGET) --version
	
daemon  :
	./$(TERGET) --daemon
