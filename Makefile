TARGET  = imgfuzz
CFLAGS  = -std=c99 -pedantic -Wall -O3
SRC_DIR = src
DEPS 	= $(SRC_DIR)/main.c $(SRC_DIR)/id.c
LIBS    = -lm
BIN_DIR = /usr/local/bin
INCLUDE = -Iinclude
OUTPUT 	= colorscheme.bmp output.bmp

all: $(TARGET)
debug: CFLAGS = -DDEBUG -g -std=c99 -pedantic -Wall

$(TARGET): $(DEPS)
	$(CC) $(CFLAGS) -o $(TARGET) $(DEPS) $(INCLUDE) $(LIBS) 

clean:
	$(RM) $(TARGET)
	$(RM) $(OUTPUT)






