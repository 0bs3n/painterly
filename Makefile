TARGET  = painterly
CFLAGS  = -std=c99 -pedantic -Wall -O3
DEPS 	= $(SRC_DIR)/painterly.c $(SRC_DIR)/siml.c
INCLUDE = include
SRC_DIR = src
LIBS    = -lm
BIN_DIR = /usr/local/bin
OUTPUT 	= colorscheme.bmp output.bmp

all: $(TARGET)


$(TARGET): $(DEPS)
	$(CC) $(CFLAGS) -o $(TARGET) $(DEPS) $(LIBS) 

clean:
	$(RM) $(TARGET)
	$(RM) $(OUTPUT)






