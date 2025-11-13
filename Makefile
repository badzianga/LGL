CC=gcc
INC=-Iinclude -Iexternal

SRC_DIR=src

BUILD_DIR=build
OUT_BASE=$(BUILD_DIR)/out

SRCS=$(wildcard $(SRC_DIR)/*.c)
LIB_NAME=libLGL.a

PLATFORM_SRC=$(SRC_DIR)/platform/PlatformLinux.c
SRCS+=$(PLATFORM_SRC)

all: debug release

debug:
	@$(MAKE) --no-print-directory MODE=debug CFLAGS="-Wall -Wextra -pedantic -ggdb" BUILD_TYPE=DEBUG prepare build

release:
	@$(MAKE) --no-print-directory MODE=release CFLAGS="-Wall -Wextra -O2 -flto" BUILD_TYPE=RELEASE prepare build

prepare:
	mkdir -p $(OUT_BASE)/$(MODE)/platform

build:
	$(MAKE) OUT_SUBDIR=$(OUT_BASE)/$(MODE) LIB_TARGET=$(OUT_BASE)/$(MODE)/$(LIB_NAME) do_build

do_build: $(patsubst $(SRC_DIR)/%.c, $(OUT_SUBDIR)/%.o, $(SRCS))
	ar rcs $(LIB_TARGET) $^

$(OUT_SUBDIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

clean:
	rm -rf $(OUT_BASE)

.PHONY: all debug release prepare build do_build clean
