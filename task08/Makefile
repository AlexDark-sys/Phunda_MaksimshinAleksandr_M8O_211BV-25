CC = gcc

CFLAGS = -Wall -Wextra -Wpedantic -Werror -std=c11 -O2 -D_FORTIFY_SOURCE=2
CPPFLAGS = -I./include
LDLIBS = -lm

TARGET = main.out
BUILD_DIR = build

all: $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(BUILD_DIR)/main.o $(BUILD_DIR)/task8.o
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o $@

$(BUILD_DIR)/main.o: main.c include/task8.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/task8.o: task8.c include/task8.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(TARGET) $(BUILD_DIR)

.PHONY: all clean