CC=g++
CFLAGS=-c -Wall
SOURCE_1=lab2_1.cpp
SOURCE_2=lab2_2.cpp
CURR_SOURCE=$(SOURCE_1)
OBJECTS=$(CURR_SOURCE:.cpp=.o)
EXECUTABLE=prog

all: $(CURR_SOURCE) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
		$(CC) $(OBJECTS) -o $@

.cpp.o:
		$(CC) $(CFLAGS) $< -o $@
