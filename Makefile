CC = clang
CSTD ?= gnu99
CFLAGS ?= -Wall -Wextra -Wpedantic
LDFLAGS ?=


all: debug
debug: ./build/debug/caked
release: ./build/release/caked
.PHONY: all debug release

OBJECTS := \
	./first.o

BOLD := $(shell tput bold)
GREEN := $(shell tput setaf 2)
RESET := $(shell tput sgr0)

./build/debug/%.o: %.c | configure
	@echo "$(GREEN)==>$(RESET) $(BOLD)cc$(RESET)   //$<"
	@$(CC) -std=$(CSTD) $(CFLAGS) -c $< -o $@

./build/debug/caked: $(foreach obj,$(OBJECTS),./build/debug/$(obj)) | configure
	@echo "$(GREEN)==>$(RESET) $(BOLD)link$(RESET) //$@"
	@$(CC) $(LDFLAGS) $^ -o $@

./build/release/%.o: %.c | configure
	@echo "$(GREEN)==>$(RESET) $(BOLD)cc$(RESET)   //$<"
	@$(CC) -std=$(CSTD) $(CFLAGS) -c $< -o $@ -O3

./build/release/caked: $(foreach obj,$(OBJECTS),./build/release/$(obj)) | configure
	@echo "$(GREEN)==>$(RESET) $(BOLD)link$(RESET) //$@"
	@$(CC) $(LDFLAGS) $^ -o $@ -O3


configure: \
	build/ \
	build/debug/ \
	build/release/
.PHONY: configure

build/:
	@mkdir build/

build/debug/: | build/
	@mkdir build/debug/

build/release/: | build/
	@mkdir build/release/


clean:
	@rm -r build/
.PHONY: clean
