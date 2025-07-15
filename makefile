CC = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0`
LIBS = `pkg-config --libs gtk+-3.0`
TARGET = PageFaultGUI
SRC = PageFaultGUI.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(LIBS)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)
