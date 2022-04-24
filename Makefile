CXX = g++
FLAG = -std=c++11 -o

CLIENT_FILE = clientA.cpp clientB.cpp
SERVER_FILE = serverM.cpp serverA.cpp serverB.cpp serverC.cpp
HEADERS = client.h server.h backend.h user.h operation.h transaction.h config.h utils.h
EXECUTABLE = clientA clientB serverM serverA serverB serverC

all: $(CLIENT_FILE) $(SERVER_FILE) $(HEADERS)
	for obj in $(EXECUTABLE) ; do $(CXX) $(FLAG) $$obj $$obj.cpp ; done