PRESET  ?= default
BUILD_DIR := build
BIN := $(BUILD_DIR)/elfpeek

.PHONY: build rebuild clean run test

$(BUILD_DIR)/CMakeCache.txt:
	cmake --preset $(PRESET)

build: $(BUILD_DIR)/CMakeCache.txt
	cmake --build --preset $(PRESET)

rebuild: clean build

clean:
	rm -rf $(BUILD_DIR)

run: build
	./$(BIN) $(FILE)
