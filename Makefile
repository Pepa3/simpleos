C_SOURCES = $(wildcard kernel/*.c kernel/std/*.c)
HEADERS = $(wildcard kernel/include/*.h)
OBJ = ${C_SOURCES:.c=.o}

build: os-image

run: os-image
	qemu-system-x86_64 -drive file=os-image,format=raw

disasm: os-image kernel.bin
	ndisasm os-image > os-image.dis
	ndisasm kernel.bin > kernel.dis

os-image: asm/main.bin kernel.bin
	cat $^ > os-image

kernel.bin: asm/kernel_entry.o asm/interrupt.o ${OBJ}
	ld -o $@ -Ttext 0x1000 -melf_i386 $^ --oformat binary

%.o: %.c ${HEADERS}
	gcc -g -fno-pic -ffreestanding -nostdinc -m32 -I./kernel/include -Wall -Wextra -c $< -o $@ 

%.o: %.asm
	nasm $< -f elf32 -o $@

%.bin: %.asm
	nasm $< -f bin -I 'asm/' -o $@

clean:
	rm -fr *.bin *.dis *.o os-image
	rm -fr kernel/*.o asm/*.bin asm/*.o kernel/std/*.o
