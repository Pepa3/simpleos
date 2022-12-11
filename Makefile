C_SOURCES = $(wildcard kernel/*.c)
HEADERS = $(wildcard kernel/*.h)
OBJ = ${C_SOURCES:.c=.o}

all: os-image

disasm: os-image kernel.bin
	ndisasm os-image > os-image.dis
	ndisasm kernel.bin > kernel.dis

os-image: asm/main.bin kernel.bin
	cat $^ > os-image

kernel.bin: kernel/kernel_entry.o ${OBJ}
	ld -o $@ -Ttext 0x1000 $^ --oformat binary -melf_i386

%.o: %.c ${HEADERS}
	gcc -fno-pic -ffreestanding -c $< -o $@ -m32

%.o: %.asm
	nasm $< -f elf32 -o $@

%.bin: %.asm
	nasm $< -f bin -I 'asm/' -o $@
	
clean:
	rm -fr *.bin *.dis *.o os-image
	rm -fr kernel/*.o asm/*.bin