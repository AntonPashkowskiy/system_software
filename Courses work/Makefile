CC = g++
CFLAGS = -c -std=c++11 -Wall -D_GNU_SOURCE 
LDFLAGS =
SOURCES = sourses/main.cpp sourses/exceptions/archive_exceptions.cpp sourses/options/argument_handler.cpp sourses/options/switcher_task.cpp sourses/archivation/masher_archivator.cpp sourses/archivation/CRC32.cpp sourses/fsystem/fs_tree.cpp sourses/compression/LZW.cpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET_PATH = /usr/bin
EXECUTABLE = masher

all: $(SOURSES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o: $(SOURSES)
	$(CC) $(CFLAGS) $< -o $@

clean: $(SOURSES)
	rm $(OBJECTS) $(EXECUTABLE)

remake: $(SOURSES)
	rm $(OBJECTS) $(EXECUTABLE)
	$(CC) $(CFLAGS) $(SOURSES)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(EXECUTABLE)

install:
	install ./$(EXECUTABLE) $(TARGET_PATH)

unistall:
	rm -rf $(TARGET_PATH)/$(EXECUTABLE) 
