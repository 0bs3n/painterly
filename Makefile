TARGET  = painterly
CFLAGS  = -std=c99 -pedantic -Wall -Ofast
DEPS 	= $(SRC_DIR)/painterly.c $(SRC_DIR)/siml.c
INCLUDE = -Iinclude
SRC_DIR = src
LIBS    = -lm
BIN_DIR = /usr/local/bin
OUTPUT 	= colorscheme.png output.png
OBJECTS = painterly.o siml.o

all: $(TARGET)

$(TARGET): painterly.o siml.o
	$(CC) $(CFLAGS) -o painterly painterly.o siml.o $(LIBS)

painterly.o: src/painterly.c
	$(CC) $(CFLAGS) -c src/painterly.c $(INCLUDE)

siml.o: src/siml.c
	$(CC) $(CFLAGS) -c src/siml.c $(INCLUDE)

clean:
	$(RM) $(TARGET)
	$(RM) $(OUTPUT)
	$(RM) $(OBJECTS)
