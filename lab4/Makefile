CC=g++
CFLAGS=-c -g -Wall -D_GNU_SOURCE 
LDFLAGS=-pthread
SOURCES=main.cpp threads_synch.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=main

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o: $(SOURCES)
	$(CC) $(CFLAGS) $< -o $@

clean: $(SOURCES)
	rm $(OBJECTS) $(EXECUTABLE)

remake: $(SOURCES)
	rm $(OBJECTS) $(EXECUTABLE)
	$(CC) $(CFLAGS) $(SOURCES)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(EXECUTABLE)
