CC      = gcc
CXX     = g++
TARGET  = SoaprunServer
# CFLAGS  ?= -fsanitize=address -g
CFLAGS  ?= -g
LDFLAGS ?= -lpthread

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

%.o: %.cpp
	$(CXX) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LDFLAGS) $(CFLAGS)

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
