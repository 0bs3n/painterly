TARGET  = painterly
CFLAGS  = -std=c99 -pedantic -Wall -Ofast
DEPS 	= $(SRC_DIR)/painterly.c $(SRC_DIR)/siml.c
INCLUDE = -Iinclude
SRC_DIR = src
LIBS    = -lm
BIN_DIR = /usr/local/bin
OUTPUT 	= colorscheme.png output.png

all: $(TARGET)


$(TARGET): $(DEPS)
	$(CC) $(CFLAGS) -o $(TARGET) $(INCLUDE) $(DEPS) $(LIBS) 

clean:
	$(RM) $(TARGET)
	$(RM) $(OUTPUT)






