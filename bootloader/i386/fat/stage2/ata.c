#include <ata.h>
#include <io.h>

/*
	ata_check_busy

	Function information:
	Waits for ATA to be ready to send/recieve data

	Parameters:
	none

	Return:
	none
*/
void ata_check_busy()
{
	while(inb(ATA_CMD_STA_REG) & BSY)
	{
		puts("Drive busy!\n");
	}
}

/*
	ata_check_ready

	Function information:
	Waits for ATA to be operative again (Not spinning)

	Parameters:
	none

	Return:
	none
*/
void ata_check_ready()
{
	while(!(inb(ATA_CMD_STA_REG) & RDY))
	{
		puts("Drive not ready!\n");
	}
}

/*
	atapio24_read

	Function information:
	Reads sectors from ATA (24 Bit LBA) to a destination address.

	Parameters:
	m_dst -> 32 bit memory address to read to
	m_lba -> 24 bit LBA addressing value to interact with the disk
	m_sectsz -> sectors to read

	Return:
	none
*/
void atapio24_read(uint8_t *m_dst, uint32_t m_lba, uint8_t m_sectsz)
{
	// Disable interrupts before reading
	__asm__ __volatile__ ("cli");

	// Needed for the nested loop
	unsigned int i, j;

	// Check if drive is busy
	//ata_check_busy();

	/*
		TODO:

		Check for slave bit and send the corresponding value automatically.
	
		For now, use a define'd comparison.
	*/
	if (ATA_DEFAULT != ATA_SECONDARY)
	{
		// Sends the command to the master drive
		outb(ATA_DRIVE_HEAD_REG, 0xE0 | ((m_lba >> 24) & 0xF));
	}
	else
	{
		// Sends the command to the slave drive
		outb(ATA_DRIVE_HEAD_REG, 0xF0 | ((m_lba >> 24) & 0xF));
	}

	// Send the sector count size to the register
	outb(ATA_SECT_CNT_REG, m_sectsz);
	
	// Sends bits [0:7] of the LBA value to the register
	outb(ATA_SECT_NUM_REG, (uint8_t) m_lba);
	
	// Sends bits [8:15] of the LBA value to the register
	outb(ATA_CYL_LO_REG, (uint8_t) (m_lba >> 8));
	
	// Sends bits [16:23] of the LBA value to the register
	outb(ATA_CYL_HI_REG, (uint8_t) (m_lba >> 16)); 
	
	// Send the read command to the register
	outb(ATA_CMD_STA_REG, ATA_READ_CMD);

	// Do this for all the sectors needed
	/*for (i = 0; i < m_sectsz; i++)
	{
		// This is painfully slow, we should avoid ATA PIO
		//ata_check_busy();
		//ata_check_ready();
		
		/*
			TODO:
			Regular sector size is 512 bytes, that doesn't mean
			that all drives will default to that so better check
			in the future for that

			For now:
			256 words (inw) = 512 bytes (Sector size)
		

		for(j = 0; j < 0x100; j++)
		{
			// Fill using Port I/O each memory location's contents
			m_dst[j] = inw(ATA_DATA_REG);
		}

		// Point to the next 512 bytes
		m_dst += 0x100;
	}*/
	// Enable interrupts after reading
	__asm__ __volatile__("sti");
}

/*
	atapio24_write

	Function information:
	Reads sectors from ATA (24 Bit LBA) to a destination address.

	Parameters:
	m_dst -> 32 bit memory address to read to
	m_lba -> 24 bit LBA addressing value to interact with the disk
	m_sectsz -> sectors to read

	Return:
	none
*/
void atapio24_write(uint32_t *m_data, uint32_t m_lba, uint8_t m_sectsz)
{
	// Disable interrupts before writing
	__asm__ __volatile__ ("cli");

	// For the nested loops
	unsigned int i, j;

	// Check if drive is busy
	ata_check_busy();

	/*
		TODO:

		Check for slave bit and send the corresponding value automatically.
	
		For now, use a define'd comparison.
	*/
	if (ATA_DEFAULT != ATA_SECONDARY)
	{
		// Sends the command to the master drive
		outb(ATA_DRIVE_HEAD_REG, 0xE0 | ((m_lba >> 24) & 0xF));
	}
	else
	{
		// Sends the command to the slave drive
		outb(ATA_DRIVE_HEAD_REG, 0xF0 | ((m_lba >> 24) & 0xF));
	}

	// Send the sector count size to the register
	outb(ATA_SECT_CNT_REG, m_sectsz);
	
	// Sends bits [0:7] of the LBA value to the register
	outb(ATA_SECT_NUM_REG, (uint8_t) m_lba);
	
	// Sends bits [8:15] of the LBA value to the register
	outb(ATA_CYL_LO_REG, (uint8_t) (m_lba >> 8));
	
	// Sends bits [16:23] of the LBA value to the register
	outb(ATA_CYL_HI_REG, (uint8_t) (m_lba >> 16)); 

	// Send the write command to the register
	outb(ATA_CMD_STA_REG, ATA_WRITE_CMD);

	// Do this for all the sectors needed
	for (i = 0; i < m_sectsz; i++)
	{
		// This is painfully slow, we should avoid ATA PIO
		ata_check_busy();
		ata_check_ready();

		/*
			TODO:
			Regular sector size is 512 bytes, that doesn't mean
			that all drives will default to that so better check
			in the future for that

			For now:
			256 words (inw) = 512 bytes (Sector size)
		*/
		for (j = 0; j < 0x100; j++)
		{
			// Write using Port I/O the data to the disk
			outl(ATA_DATA_REG, m_data[j]);
		}
	}

	// Enable interrupts after writing
	__asm__ __volatile__("sti");
}
