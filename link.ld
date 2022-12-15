ENTRY(_start)
SECTIONS
{
	. = 1M;

	kernel_base = .;

	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.text)
	}
	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}
	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}
	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(.bss)
		QUAD(0xbeefcafe)
	}

	kernel_end = .;
}
