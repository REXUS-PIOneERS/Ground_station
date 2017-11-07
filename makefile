GND_TARGET = bin/groundstation

CXX = g++
INCPATH = -I. -I./include -I../Radio_comm/include
CFLAGS = -Wall -c -std=c++11
LFLAGS = -Wall -pthread
DEBUG =

COMPILE = $(CXX) $(CFLAGS) $(INCPATH) $^ -o $@
LINK = $(CXX) $(LFLAGS) $^ -o $@


$(GND_TARGET): build/main.o build/cmd.o build/workers.o build/radiocom.o build/protocol.o
	$(LINK)
build/main.o: src/main.cpp
	$(COMPILE)
build/cmd.o: src/cmd.cpp
	$(COMPILE)
build/workers.o: src/workers.cpp
	$(COMPILE)
build/radiocom.o: ../Radio_comm/src/radiocom.cpp
	$(COMPILE)
build/protocol.o: ../Radio_comm/src/protocol.cpp
	$(COMPILE)


$(shell mkdir -p bin)
$(shell mkdir -p build)

clean:
	rm -frv build/*
