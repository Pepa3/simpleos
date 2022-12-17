/** @author Levente Kurusa <levex@linux.com> **/

#include <stdint.h>
#include <stdio.h>
#include <disk.h>
#include <ports.h>
#include <screen.h>
#include <isr.h>
#include <memory.h>

uint8_t ata_pm = 0; /* Primary master exists? */
uint8_t ata_ps = 0; /* Primary Slave exists? */
uint8_t ata_sm = 0; /* Secondary master exists? */
uint8_t ata_ss = 0; /* Secondary slave exists? */

uint8_t *ide_buf = 0;

device_t* disks[4];//PM,PS,SM,SS

void ide_select_drive(uint8_t bus, uint8_t i)
{
	if(bus == ATA_PRIMARY)
		if(i == ATA_MASTER)
			port_byte_out(ATA_PRIMARY_IO + ATA_REG_HDDEVSEL, 0xA0);
		else port_byte_out(ATA_PRIMARY_IO + ATA_REG_HDDEVSEL, 0xB0);
	else
		if(i == ATA_MASTER)
			port_byte_out(ATA_SECONDARY_IO + ATA_REG_HDDEVSEL, 0xA0);
		else port_byte_out(ATA_SECONDARY_IO + ATA_REG_HDDEVSEL, 0xB0);
}

void ide_primary_irq()
{
	asm volatile("add $0x1c, %esp");
	asm volatile("pusha");
	send_eoi(ATA_PRIMARY_IRQ);
	asm volatile("popa");
	asm volatile("iret");
}

void ide_secondary_irq()
{
	asm volatile("add $0x1c, %esp");
    asm volatile("pusha");
	send_eoi(ATA_SECONDARY_IRQ);
	asm volatile("popa");
	asm volatile("iret");
}

uint8_t ide_identify(uint8_t bus, uint8_t drive)
{
	uint16_t io = 0;
	ide_select_drive(bus, drive);
	if(bus == ATA_PRIMARY) io = ATA_PRIMARY_IO;
	else io = ATA_SECONDARY_IO;
	/* ATA specs say these values must be zero before sending IDENTIFY */
	port_byte_out(io + ATA_REG_SECCOUNT0, 0);
	port_byte_out(io + ATA_REG_LBA0, 0);
	port_byte_out(io + ATA_REG_LBA1, 0);
	port_byte_out(io + ATA_REG_LBA2, 0);
	/* Now, send IDENTIFY */
	port_byte_out(io + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
	print("Sent IDENTIFY\n");
	/* Now, read status port */
	uint8_t status = port_byte_in(io + ATA_REG_STATUS);
	if(status)
	{
		/* Now, poll untill BSY is clear. */
		while((port_byte_in(io + ATA_REG_STATUS) & ATA_SR_BSY) != 0) ;
pm_stat_read:		status = port_byte_in(io + ATA_REG_STATUS);
		if(status & ATA_SR_ERR)
		{
			printf("%s%s has ERR set. Disabled.\n", bus==ATA_PRIMARY?"Primary":"Secondary", drive==ATA_PRIMARY?" master":" slave");
			return 0;
		}
		while(!(status & ATA_SR_DRQ)) goto pm_stat_read;
		printf("%s%s is online.\n", bus==ATA_PRIMARY?"Primary":"Secondary", drive==ATA_PRIMARY?" master":" slave");
		/* Now, actually read the data */
		//set_task(0);
		for(int i = 0; i<256; i++)
		{
			*(uint16_t *)(ide_buf + i*2) = port_word_in(io + ATA_REG_DATA);
		}
		//set_task(1);
        return 1;
	}
    return 0;
}

void ide_400ns_delay(uint16_t io)
{
	for(int i = 0;i < 4; i++)
		port_byte_in(io + ATA_REG_ALTSTATUS);
}

void ide_poll(uint16_t io)
{
	
	for(int i=0; i< 4; i++)
		port_byte_in(io + ATA_REG_ALTSTATUS);

retry:;
	uint8_t status = port_byte_in(io + ATA_REG_STATUS);
	//print("testing for BSY\n");
	if(status & ATA_SR_BSY) goto retry;
	//print("BSY cleared\n");
retry2:	status = port_byte_in(io + ATA_REG_STATUS);
	if(status & ATA_SR_ERR)
	{
		print("ERR set, device failure!\n");
	}
	//print("testing for DRQ\n");
	if(!(status & ATA_SR_DRQ)) goto retry2;
	//print("DRQ set, ready to PIO!\n");
	return;
}

uint8_t ata_read_one(uint16_t *buf, uint32_t offset, device_t *dev)
{
	//offset &= 0x00FFFFFF; // ignore topmost byte
	/* We only support 28bit LBA so far */
	uint8_t drive = ((ide_private_data *)(dev->priv))->drive;
	uint16_t io = 0;
	switch(drive)
	{
		case (ATA_PRIMARY << 1 | ATA_MASTER):
			io = ATA_PRIMARY_IO;
			drive = ATA_MASTER;
			break;
		case (ATA_PRIMARY << 1 | ATA_SLAVE):
			io = ATA_PRIMARY_IO;
			drive = ATA_SLAVE;
			break;
		case (ATA_SECONDARY << 1 | ATA_MASTER):
			io = ATA_SECONDARY_IO;
			drive = ATA_MASTER;
			break;
		case (ATA_SECONDARY << 1 | ATA_SLAVE):
			io = ATA_SECONDARY_IO;
			drive = ATA_SLAVE;
			break;
		default:
			print("FATAL: unknown drive!\n");
			return 0;
	}
	//print("io=0x%x %s\n", io, drive==ATA_MASTER?"Master":"Slave");
	uint8_t cmd = (drive==ATA_MASTER?0xE0:0xF0);
	//uint8_t slavebit = (drive == ATA_MASTER?0x00:0x01);
	/*print("offset = 0x%x\n", offset);
	print("offset>>24 & 0x0f = %d\n", (offset >> 24)&0x0f);
	print("(uint8_t)offset = %d\n", (uint8_t)offset);
	print("(uint8_t)(offset >> 8) = %d\n", (uint8_t)(offset >> 8));
	print("(uint8_t)(offset >> 16) = %d\n", (uint8_t)(offset >> 16));*/
	//port_byte_out(io + ATA_REG_HDDEVSEL, cmd | ((offset >> 24)&0x0f));
	port_byte_out(io + ATA_REG_HDDEVSEL, (cmd | (uint8_t)((offset >> 24 & 0x0F))));
	//print("issued 0x%x to 0x%x\n", (cmd | (offset >> 24)&0x0f), io + ATA_REG_HDDEVSEL);
	//for(int k = 0; k < 10000; k++) ;
	port_byte_out(io + 1, 0x00);
	//print("issued 0x%x to 0x%x\n", 0x00, io + 1);
	//for(int k = 0; k < 10000; k++) ;
	port_byte_out(io + ATA_REG_SECCOUNT0, 1);
	//print("issued 0x%x to 0x%x\n", (uint8_t) numsects, io + ATA_REG_SECCOUNT0);
	//for(int k = 0; k < 10000; k++) ;
	port_byte_out(io + ATA_REG_LBA0, (uint8_t)((offset)));
	//print("issued 0x%x to 0x%x\n", (uint8_t)((offset)), io + ATA_REG_LBA0);
	//for(int k = 0; k < 10000; k++) ;
	port_byte_out(io + ATA_REG_LBA1, (uint8_t)((offset) >> 8));
	//print("issued 0x%x to 0x%x\n", (uint8_t)((offset) >> 8), io + ATA_REG_LBA1);
	//for(int k = 0; k < 10000; k++) ;
	port_byte_out(io + ATA_REG_LBA2, (uint8_t)((offset) >> 16));
	//print("issued 0x%x to 0x%x\n", (uint8_t)((offset) >> 16), io + ATA_REG_LBA2);
	//for(int k = 0; k < 10000; k++) ;
	port_byte_out(io + ATA_REG_COMMAND, ATA_CMD_READ_PIO);
	//print("issued 0x%x to 0x%x\n", ATA_CMD_READ_PIO, io + ATA_REG_COMMAND);

	ide_poll(io);

	//set_task(0);
	for(int i = 0; i < 256; i++)
	{
		buf[i] = port_word_in(io + ATA_REG_DATA);
	}
	ide_400ns_delay(io);
	//set_task(1);
	return 1;
}

uint8_t ata_write_one(uint16_t* buf, uint32_t offset, device_t *dev){
	//offset &= 0x00FFFFFF; // ignore topmost byte
	/* We only support 28bit LBA so far */
	uint8_t drive = ((ide_private_data *)(dev->priv))->drive;
	uint16_t io = 0;
	switch(drive)
	{
		case (ATA_PRIMARY << 1 | ATA_MASTER):
			io = ATA_PRIMARY_IO;
			drive = ATA_MASTER;
			break;
		case (ATA_PRIMARY << 1 | ATA_SLAVE):
			io = ATA_PRIMARY_IO;
			drive = ATA_SLAVE;
			break;
		case (ATA_SECONDARY << 1 | ATA_MASTER):
			io = ATA_SECONDARY_IO;
			drive = ATA_MASTER;
			break;
		case (ATA_SECONDARY << 1 | ATA_SLAVE):
			io = ATA_SECONDARY_IO;
			drive = ATA_SLAVE;
			break;
		default:
			print("FATAL: unknown drive!\n");
			return 0;
	}
	//print("io=0x%x %s\n", io, drive==ATA_MASTER?"Master":"Slave");
	uint8_t cmd = (drive==ATA_MASTER?0xE0:0xF0);
	//uint8_t slavebit = (drive == ATA_MASTER?0x00:0x01);
	/*print("LBA = 0x%x\n", offset);
	print("LBA>>24 & 0x0f = %d\n", (offset >> 24)&0x0f);
	print("(uint8_t)offset = %d\n", (uint8_t)offset);
	print("(uint8_t)(offset >> 8) = %d\n", (uint8_t)(offset >> 8));
	print("(uint8_t)(offset >> 16) = %d\n", (uint8_t)(offset >> 16));*/
	//port_byte_out(io + ATA_REG_HDDEVSEL, cmd | ((offset >> 24)&0x0f));
	port_byte_out(io + ATA_REG_HDDEVSEL, (cmd | (uint8_t)((offset >> 24 & 0x0F))));
	//print("issued 0x%x to 0x%x\n", (cmd | (offset >> 24)&0x0f), io + ATA_REG_HDDEVSEL);
	//for(int k = 0; k < 10000; k++) ;
	port_byte_out(io + 1, 0x00);
	//print("issued 0x%x to 0x%x\n", 0x00, io + 1);
	//for(int k = 0; k < 10000; k++) ;
	port_byte_out(io + ATA_REG_SECCOUNT0, 1);
	//print("issued 0x%x to 0x%x\n", (uint8_t) numsects, io + ATA_REG_SECCOUNT0);
	//for(int k = 0; k < 10000; k++) ;
	port_byte_out(io + ATA_REG_LBA0, (uint8_t)((offset)));
	//print("issued 0x%x to 0x%x\n", (uint8_t)((offset)), io + ATA_REG_LBA0);
	//for(int k = 0; k < 10000; k++) ;
	port_byte_out(io + ATA_REG_LBA1, (uint8_t)((offset) >> 8));
	//print("issued 0x%x to 0x%x\n", (uint8_t)((offset) >> 8), io + ATA_REG_LBA1);
	//for(int k = 0; k < 10000; k++) ;
	port_byte_out(io + ATA_REG_LBA2, (uint8_t)((offset) >> 16));
	//print("issued 0x%x to 0x%x\n", (uint8_t)((offset) >> 16), io + ATA_REG_LBA2);
	//for(int k = 0; k < 10000; k++) ;
	port_byte_out(io + ATA_REG_COMMAND, ATA_CMD_WRITE_PIO);
	//print("issued 0x%x to 0x%x\n", ATA_CMD_READ_PIO, io + ATA_REG_COMMAND);

	ide_poll(io);

	//set_task(0);
	for(int i = 0; i < 256; i++)
	{
		port_word_out(io + ATA_REG_DATA,buf[i]);
	}
	ide_400ns_delay(io);
	//set_task(1);
	return 1;
}

uint8_t ata_read(uint16_t *buf, uint32_t offset, uint32_t numsects, device_t *dev)
{	
	uint8_t status = 1;
	for(uint32_t i = 0; i < numsects; i++)
	{
		status &= ata_read_one(buf, offset + i, dev);
		buf += 512;
	}
	return status;
}

uint8_t ata_write(uint16_t *buf, uint32_t offset, uint32_t numsects, device_t *dev)
{	
	uint8_t status = 1;
	for(uint32_t i = 0; i < numsects; i++)
	{
		status &= ata_write_one(buf, offset + i, dev);
		buf += 512;
	}
	return status;
}

void ata_probe()
{
	/* First check the primary bus,
	 * and inside the master drive.
	 */
	if(ide_identify(ATA_PRIMARY, ATA_MASTER))
	{
		ata_pm = 1;
		device_t *dev = (device_t *)malloc(sizeof(device_t));
		ide_private_data *priv = (ide_private_data *)malloc(sizeof(ide_private_data));
		/* Now, process the IDENTIFY data */
		/* Model goes from W#27 to W#46 */
		char *str = (char *)malloc(40);
		for(int i = 0; i < 40; i += 2)
		{
			str[i] = ide_buf[ATA_IDENT_MODEL + i + 1];
			str[i + 1] = ide_buf[ATA_IDENT_MODEL + i];
		}
		dev->name = str;
		dev->unique_id = 32;
		dev->dev_type = DEVICE_BLOCK;
		priv->drive = (ATA_PRIMARY << 1) | ATA_MASTER;
		dev->priv = priv;
		dev->read = ata_read;
		dev->write = ata_write_one;
		//device_add(dev);
		disks[0]=dev;
		printf("Device: %s\n", dev->name);
	}
	if(ide_identify(ATA_PRIMARY, ATA_SLAVE))
	{
		ata_ps = 1;
		device_t *dev = (device_t *)malloc(sizeof(device_t));
		ide_private_data *priv = (ide_private_data *)malloc(sizeof(ide_private_data));
		/* Now, process the IDENTIFY data */
		/* Model goes from W#27 to W#46 */
		char *str = (char *)malloc(40);
		for(int i = 0; i < 40; i += 2)
		{
			str[i] = ide_buf[ATA_IDENT_MODEL + i + 1];
			str[i + 1] = ide_buf[ATA_IDENT_MODEL + i];
		}
		dev->name = str;
		dev->unique_id = 32;
		dev->dev_type = DEVICE_BLOCK;
		priv->drive = (ATA_PRIMARY << 1) | ATA_SLAVE;
		dev->priv = priv;
		dev->read = ata_read;
		dev->write = ata_write_one;
		//device_add(dev);
		disks[1]=dev;
		printf("Device: %s\n", dev->name);
	}
	/*ide_identify(ATA_SECONDARY, ATA_MASTER);
	ide_identify(ATA_SECONDARY, ATA_SLAVE);*/
}

device_t* get_disk(int n){
	if(n<0 || n>3){return NULL;}

	if((n==0) & ata_pm){
		return disks[0];

	}else if((n==1) & ata_ps){
		return disks[1];

	}else if((n==2) & ata_sm){
		return disks[2];

	}else if((n==3) & ata_ss){
		return disks[3];
	}
	return NULL;
}

device_t* ata_init()
{
	print("Checking for ATA drives\n");
	ide_buf = malloc(512);
	register_interrupt_handler(ATA_PRIMARY_IRQ, ide_primary_irq);
	register_interrupt_handler(ATA_SECONDARY_IRQ, ide_secondary_irq);
	ata_probe();
	return disks[0];
}