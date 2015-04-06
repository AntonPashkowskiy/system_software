CC=g++
CFLAGS=-c -std=c++11 -g -Wall -D_GNU_SOURCE 
LDFLAGS=-pthread
SOURCES=sourses/main.cpp sourses/options/argument_handler.cpp sourses/exceptions/archive_exceptions.cpp sourses/options/switcher_task.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=masher

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
