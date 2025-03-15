# Compiler and flags
CC = g++
CFLAGS = -Wall -O2 -I/boot/system/develop/headers/private/interface
LDFLAGS = -lbe -ltranslation

# Source and output files
SRC = HEICTranslator.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = HEICTranslator.so

# Default target
all: $(TARGET)

# Build the translator shared object
$(TARGET): $(OBJ)
	$(CC) -shared -o $(TARGET) $(OBJ) $(LDFLAGS)

# Compile source files
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJ) $(TARGET)

# Install the translator
install: $(TARGET)
	cp $(TARGET) /boot/system/add-ons/Translators/

# Restart the TranslatorRoster
restart:
	kill -9 `ps | grep "TranslatorRoster" | awk '{print $1}'`