#include<sys/ahci/ahcicommands.h>
#include<sys/ahci/fis.h>
#include<sys/memory/free_phys_pages.h>

int find_cmdslot(HBA_PORT *port);

int read_ahci(HBA_PORT *port, DWORD startl, DWORD starth, DWORD count, WORD *buffer){
	printf("port->is %p", port->is);
	DWORD countsectors = count;
	port->is = 0xFFFFFFFF;		// Clear pending interrupt bits
	printf("port->is %p", port->is);
	int spin = 0; // Spin lock timeout counter
	int slot = find_cmdslot(port);
	if (slot == -1)
	return -1;
	

	uint64_t clb = port->clb;
	uint64_t clb_vir = v_alloc_page_get_phys(&clb, PAGE_TRANS_READ_WRITE | PAGE_TRANS_USER_SUPERVISOR);
	HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)clb_vir;
	//TODO: how to get the virtual address
	cmdheader += slot;
	cmdheader->cfl = sizeof(FIS_REG_H2D)/sizeof(DWORD);	// Command FIS size
	cmdheader->w = 0;		// Read from device
	cmdheader->prdtl = (WORD)((count-1)>>4) + 1;	// PRDT entries count
	
	uint64_t ctba = cmdheader->ctba;
	uint64_t ctba_vir = v_alloc_page_get_phys(&ctba, PAGE_TRANS_READ_WRITE | PAGE_TRANS_USER_SUPERVISOR);
	HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL*)ctba_vir;
	//TODO: how to get the virtual address
	memset(cmdtbl, 0, sizeof(HBA_CMD_TBL) + (cmdheader->prdtl-1)*sizeof(HBA_PRDT_ENTRY));

	// 8K bytes (16 sectors) per PRDT
	int i=0;
	uint64_t buf = (uint64_t)buffer;
	for (i=0; i<cmdheader->prdtl-1; i++)
	{
		//TODO:dba stores physical address
		cmdtbl->prdt_entry[i].dba = (DWORD)buf;
		cmdtbl->prdt_entry[i].dbc = 8*1024 -1;	// 8K bytes
		cmdtbl->prdt_entry[i].i = 0;
		buf += 4*1024;	// 4K words
		count -= 16;	// 16 sectors
	}
	// Last entry
	cmdtbl->prdt_entry[i].dba = (DWORD)buf;
	cmdtbl->prdt_entry[i].dbc = ((count<<9) -1);	// 512 bytes per sector
	cmdtbl->prdt_entry[i].i = 0;

	// Setup command
	FIS_REG_H2D *cmdfis = (FIS_REG_H2D*)(&cmdtbl->cfis);

	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;	// Command
	cmdfis->command = ATA_CMD_READ_DMA_EX;

	cmdfis->lba0 = (BYTE)startl;
	cmdfis->lba1 = (BYTE)(startl>>8);
	cmdfis->lba2 = (BYTE)(startl>>16);
	cmdfis->device = 1<<6;	// LBA mode

	cmdfis->lba3 = (BYTE)(startl>>24);
	cmdfis->lba4 = (BYTE)starth;
	cmdfis->lba5 = (BYTE)(starth>>8);
	
	//TODO:count has a number less than 16. whats happening here?
	cmdfis->countl = LOBYTE(countsectors);
	cmdfis->counth = HIBYTE(countsectors);
	printf("Deepa here");
	// The below loop waits until the port is no longer busy before issuing a new command
	while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
	{
		spin++;
	}
	if (spin == 1000000)
	{
		printf("Port is hung\n");
		return -1;
	}
	
	printf("port->ci: %p",port->ci);
	port->ci = 1<<slot;	// Issue command
	printf("port->ci: %p",port->ci);

	// Wait for completion
	while (1)
	{
	// In some longer duration reads, it may be helpful to spin on the DPS bit 
	// in the PxIS port field as well (1 << 5)
		if ((port->ci & (1<<slot)) == 0) 
			break;
		if (port->is & HBA_PORT_IS_TFES )	// Task file error
		{
			printf("Read disk error\n");
			return -1;
		}
//		printf("Deepa is here");
	}

	// Check again
	if (port->is & HBA_PORT_IS_TFES )
	{
		printf("Read disk error\n");
		return -1;
	}

	return 0;
}

int write_ahci(HBA_PORT *port, DWORD startl, DWORD starth, DWORD count, WORD *buffer){
	port->is = (DWORD)-1;		// Clear pending interrupt bits
	int spin = 0; // Spin lock timeout counter
	int slot = find_cmdslot(port);
	printf("slot: %d", slot);
	if (slot == -1)
	return -1;
	

	uint64_t clb = port->clb;
	uint64_t clb_vir = v_alloc_page_get_phys(&clb, PAGE_TRANS_READ_WRITE | PAGE_TRANS_USER_SUPERVISOR);
	HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)clb_vir;
	//TODO: how to get the virtual address
	cmdheader += slot;
	cmdheader->cfl = sizeof(FIS_REG_H2D)/sizeof(DWORD);	// Command FIS size
	cmdheader->w = 1;		// write from device
	cmdheader->prdtl = (WORD)((count-1)>>4) + 1;	// PRDT entries count
	
	uint64_t ctba = cmdheader->ctba;
	uint64_t ctba_vir = v_alloc_page_get_phys(&ctba, PAGE_TRANS_READ_WRITE | PAGE_TRANS_USER_SUPERVISOR);
	HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL*)ctba_vir;
	//TODO: how to get the virtual address
	memset(cmdtbl, 0, sizeof(HBA_CMD_TBL) + (cmdheader->prdtl-1)*sizeof(HBA_PRDT_ENTRY));

	// 8K bytes (16 sectors) per PRDT
	int i;
	uint64_t buf = (uint64_t)buffer;
	for (i=0; i<cmdheader->prdtl-1; i++)
	{
		//TODO:dba stores physical address
		cmdtbl->prdt_entry[i].dba = (DWORD)buf;
		cmdtbl->prdt_entry[i].dbc = 8*1024 -1;	// 8K bytes
		cmdtbl->prdt_entry[i].i = 0;
		buf += 4*1024;	// 4K words
		count -= 16;	// 16 sectors
	}
	// Last entry
	cmdtbl->prdt_entry[i].dba = (DWORD)buf;
	cmdtbl->prdt_entry[i].dbc = ((count<<9) -1);	// 512 bytes per sector
	cmdtbl->prdt_entry[i].i = 0;

	// Setup command
	FIS_REG_H2D *cmdfis = (FIS_REG_H2D*)(&cmdtbl->cfis);

	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;	// Command
	cmdfis->command = ATA_CMD_WRITE_DMA_EX;

	cmdfis->lba0 = (BYTE)startl;
	cmdfis->lba1 = (BYTE)(startl>>8);
	cmdfis->lba2 = (BYTE)(startl>>16);
	cmdfis->device = 1<<6;	// LBA mode

	cmdfis->lba3 = (BYTE)(startl>>24);
	cmdfis->lba4 = (BYTE)starth;
	cmdfis->lba5 = (BYTE)(starth>>8);
	
	//TODO:count has a number less than 16. whats happening here?
	cmdfis->countl = LOBYTE(count);
	cmdfis->counth = HIBYTE(count);
	

	// The below loop waits until the port is no longer busy before issuing a new command
	while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
	{
		spin++;
	}
	if (spin == 1000000)
	{
		printf("Port is hung\n");
		return -1;
	}

	port->ci = 1<<slot;	// Issue command

	// Wait for completion
	while (1)
	{
	// In some longer duration reads, it may be helpful to spin on the DPS bit 
	// in the PxIS port field as well (1 << 5)
		if ((port->ci & (1<<slot)) == 0) 
			break;
		if (port->is & HBA_PORT_IS_TFES )	// Task file error
		{
			printf("Read disk error\n");
			return -1;
		}
		//printf("Deepa here");
	}

	// Check again
	if (port->is & HBA_PORT_IS_TFES )
	{
		printf("Read disk error\n");
		return -1;
	}

	return 0;
}


// Find a free command list slot
int find_cmdslot(HBA_PORT *port)
{
// If not set in SACT and CI, the slot is free
	DWORD slots = (port->sact | port->ci);
	printf("port->sact: %p",port->sact);
	printf("port->ci: %p",port->ci);
	printf("slots: %p",slots);

	int cmdslots = 32; // TODO: assuming no: of command slots is 32
	for (int i=0; i<cmdslots; i++)
	{
		if ((slots&1) == 0)
			return i;
		slots >>= 1;
	}
	printf("Cannot find free command list entry\n");
	return -1;
}



