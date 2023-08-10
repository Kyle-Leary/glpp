CC := gcc
CFLAGS := 

TARGET := glpp

SRC_DIR := src
SRCS := $(shell find $(SRC_DIR) -type f)
OBJS := $(patsubst %.c,%.o,$(SRCS))

all: $(TARGET)

# level 800 makefile bullshit
TESTS := tests/test.vs tests/test.fs tests/test_common.glinc
OUTPUTS := $(patsubst %,%.pp,$(TESTS))
test: $(OUTPUTS)
	@echo "Running test on $< with the executable ./$(TARGET)..."
%.pp: %
	@./$(TARGET) $< -o $@

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f **/*.o
	rm -f **/*.pp

.PHONY: test clean
