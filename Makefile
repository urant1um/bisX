CC = gcc
CFLAGS = -m32 -ffreestanding -fno-pie -fno-stack-protector -O2 -I src/kernel

all: dirs build/bisX.bin

dirs:
	mkdir -p build

build/storage.o: src/kernel/storage.c
	$(CC) $(CFLAGS) -c src/kernel/storage.c -o build/storage.o

build/net.o: src/kernel/net.c
	$(CC) $(CFLAGS) -c src/kernel/net.c -o build/net.o

build/commands.o: src/kernel/commands.c
	$(CC) $(CFLAGS) -c src/kernel/commands.c -o build/commands.o

build/kernel.o: src/kernel/kernel.c
	$(CC) $(CFLAGS) -c src/kernel/kernel.c -o build/kernel.o

build/entry.o: src/kernel/entry.asm
	nasm -f elf32 src/kernel/entry.asm -o build/entry.o

build/boot.bin: src/boot/bscx.asm
	nasm -f bin src/boot/bscx.asm -o build/boot.bin

build/bisX.bin: build/boot.bin build/entry.o build/kernel.o build/net.o build/storage.o build/commands.o
	ld -m elf_i386 -Ttext 0x1000 --oformat binary build/entry.o build/kernel.o build/net.o build/storage.o build/commands.o -o build/kernel.bin
	cat build/boot.bin build/kernel.bin > boot/bisX.bin

clean:
	rm -rf build boot/bisX.bin
