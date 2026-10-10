CC = gcc

CFLAGS = -Wall -Wextra -Wpedantic -Werror -std=c11 -O2 -D_FORTIFY_SOURCE=2
CPPFLAGS = -I./include
LDLIBS = -lm

TARGET = main.out
BUILD_DIR = build

OBJECTS = $(BUILD_DIR)/main.o \
          $(BUILD_DIR)/series.o \
          $(BUILD_DIR)/integrals.o

all: $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o $@

$(BUILD_DIR)/%.o: %.c include/task5.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(TARGET) $(BUILD_DIR)

.PHONY: all clean