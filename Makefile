CXX = g++
FLAG = -std=c++11 -o

CLIENT_FILE = clientA.cpp clientB.cpp
SERVER_FILE = serverM.cpp serverA.cpp serverB.cpp serverC.cpp
BLOCK_FILE = block1.txt block2.txt block3.txt
HEADERS = client.h server.h backend.h user.h operation.h transaction.h config.h utils.h
EXECUTABLE = clientA clientB serverM serverA serverB serverC

all: $(BLOCK_FILE) $(SERVER_FILE) $(HEADERS) $(BLOCK_FILE)
	for obj in $(EXECUTABLE) ; do $(CXX) $(FLAG) $$obj $$obj.cpp ; done