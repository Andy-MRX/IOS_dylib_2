# macOS + Xcode 编译 arm64 + arm64e fat dylib（Apple clang 生成真正 PAC 代码）
TARGET := payload.dylib
SDK    := $(shell xcrun --sdk iphoneos --show-sdk-path)
CC     := xcrun --sdk iphoneos clang
INSTALL_NAME := /var/jb/Library/MobileSubstrate/DynamicLibraries/payload.dylib

ARCHS  := -arch arm64 -arch arm64e
CFLAGS := -O2 -Wall -fPIC
LDFLAGS := -dynamiclib -isysroot $(SDK) -install_name $(INSTALL_NAME)

all: $(TARGET)

$(TARGET): reverse_shell.c
	$(CC) $(ARCHS) $(CFLAGS) $(LDFLAGS) -o $@ reverse_shell.c
	ldid -S $@

clean:
	rm -f $(TARGET)
